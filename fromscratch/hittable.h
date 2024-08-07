#ifndef HITTABLE_H
#define HITTABLE_H

class material;

struct hit_record{
	point3 p;
	vec3 normal;
	shared_ptr<material> mat;
	double t;
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
};

#endif
