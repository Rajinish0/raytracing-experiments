
#include "rtweekend.h"

#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"


#include <algorithm>


class bvh_node : public hittable {
public:
    bvh_node(hittable_list& list) 
        :bvh_node(list.objects, 0, list.objects.size()) {}
    
    bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end)
    {
        int axis = random_int(0, 2);
        
        auto comparator = (axis == 0) ? box_compare<0>
                        : (axis == 1) ? box_compare<1>
                                      : box_compare<2>;

        
        size_t span = end - start;

        if (span == 1) left = right = objects[start];
        else if (span == 2) {
            left = objects[start];
            right = objects[start + 1];
        } else {
            std::sort(
                std::begin(objects) + start,
                std::begin(objects) + end, 
                comparator
            );

            auto mid = (start + end) / 2;
            left = std::make_shared<bvh_node>(objects, start, mid);
            right = std::make_shared<bvh_node>(objects, mid, end);

            bbox = aabb(left->bounding_box(), right->bounding_box());
        }
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override{
        if (!bbox.hit(r, ray_t)) return false; // early culling

        bool hit_left = left->hit(r, ray_t, rec);
        bool hit_right = right->hit(r,  interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

        return hit_left || hit_right;
    }

    aabb bounding_box() const override {return bbox;}

private:
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb bbox;

    template <int axis>
    std::enable_if_t<axis<=2, bool>
    static box_compare(const shared_ptr<hittable>& a, const shared_ptr<hittable> b){
        auto a_axis_itvl = a->bounding_box().axis_interval(axis);
        auto b_axis_itvl = b->bounding_box().axis_interval(axis);
        return a_axis_itvl.min < b_axis_itvl.min;
    }
};