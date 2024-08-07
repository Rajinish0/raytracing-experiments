#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"

#include <vector>

using std::make_shared;
using std::shared_ptr;

class hittable_list : public hittable{
public:
	std::vector<shared_ptr<hittable>> objects;

	hittable_list() {}
	hittable_list(shared_ptr<hittable> object) {
		add(object);
	}

	void clear(){objects.clear();}
	void add(shared_ptr<hittable> object){
		objects.push_back(object);
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		hit_record tmp_rec;
		bool hit_anything = false;
		auto closest = ray_t.max;

		for (auto const& obj : objects){
			if (obj->hit(r, interval(ray_t.min, closest), tmp_rec)){
				hit_anything = true;
				closest = tmp_rec.t;
				rec = tmp_rec;
			}
		}
		return hit_anything;
	}
};

#endif
