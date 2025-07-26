#ifndef HITTABLE_H
#define HITTABLE_H

#include "aabb.h"
#include "mat3.h"

class material;

struct hit_record{
	point3 p;
	vec3 normal;
	shared_ptr<material> mat;
	double t;
	double u, v;
	bool front_face;

	void set_front_face(const ray& r, const vec3& outward_normal){
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = (front_face) ? outward_normal : -outward_normal;
	}
};

class hittable{
public:
	virtual ~hittable() = default;
	virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const =0; // pure virtual function
	virtual aabb bounding_box() const = 0;
};


// I think this is really clever
// the existing objects do not need to be modified this way
// and all the rotation/tranlsation is abstracted away
/*
An important question: 
why does the aabb need to be changed?
Because the ray is still in word space 
when the aabb intersection of this object is 
checked, so the aabb must correspond to 
where it will be when it is translated.
*/
class translate : public hittable {
public:
	translate(shared_ptr<hittable> object, vec3 offset)
		:object(object), offset(offset), bbox(object->bounding_box() + offset) 
		{}

	aabb bounding_box() const {
		return bbox;
	}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override
	{
		ray new_r(r.origin() - offset, r.direction(), r.time());

		if (!object->hit(new_r, ray_t, rec))
			return false;
		
		rec.p += offset;

		return true;
	}

private:
	shared_ptr<hittable> object;
	vec3 offset;
	aabb bbox;
};


template<size_t axis> // axis \in [0, 1, 2]
class rotate : public hittable
{
public: 

	rotate(shared_ptr<hittable> object, double theta) 
		:object(object), theta(theta), bbox(object->bounding_box()), 
		rot_mat(mat3::rot<axis>(theta)), inv_rot(mat3::rot<axis>(-theta))
	{

		point3 min(infinity);
		point3 max(-infinity);

		// iterate through the 6 vertices
		// apply rotation on them
		// get the min, max
		for (int i =0; i < 2; ++i){
			for (int j =0; j < 2; ++j){
				for (int k =0; k < 2; ++k){
					vec3 p = vec3(i*bbox.x.max + (1-i)*bbox.x.min, 
								  j*bbox.y.max + (1-j)*bbox.y.min, 
								  k*bbox.z.max + (1-k)*bbox.z.min);
					
					vec3 rotated_p = rot_mat*p;

					for (int c = 0; c < 3; ++c){
						min[c] = std::fmin(min[c], rotated_p[c]);
						max[c] = std::fmax(max[c], rotated_p[c]);
					}
				}
			}
		}

		bbox = aabb(min, max);

	}

	// it's okay if i have this only for one function
	// because i only want to raise a compiler error
	std::enable_if_t<axis<=2, bool>
	hit(const ray& r, interval ray_t, hit_record& rec) const override{
		vec3 new_ray_orig = inv_rot * r.origin();
		vec3 new_ray_dir  = inv_rot * r.direction();

		ray new_ray(new_ray_orig, new_ray_dir, r.time());

		if (!object->hit(new_ray, ray_t, rec))
			return false;

		rec.p = rot_mat * rec.p;
		rec.normal = rot_mat * rec.normal;

		return true;
	}

	aabb bounding_box() const override {
		return bbox;
	}

private:
	shared_ptr<hittable> object;
	double theta;
	aabb bbox;
	mat3 rot_mat, inv_rot;

};

#endif
