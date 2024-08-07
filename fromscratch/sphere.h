#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : public hittable {
public:
	sphere(const point3& center, double radius, shared_ptr<material> mat)
		:center(center), radius(std::fmax(0, radius)), mat(mat){}

	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		vec3 q = center - r.origin();
		auto a = r.direction().length_squared();
		auto h = dot(r.direction(), q);
		auto c = q.length_squared() - radius * radius;

		auto disc = h*h - a*c;
		if (disc < 0)
			return false;
		
		auto dsqrt = std::sqrt(disc);

		auto root = (h-dsqrt) / a;
		if (!ray_t.surrounds(root)){
		root = (h + dsqrt)/a;
		if (!ray_t.surrounds(root))
			return false;
		}

		rec.p = r.at(root);
		rec.t = root;
		vec3 o_normal = (rec.p - center)/radius;
		rec.set_front_face(r, o_normal);
		rec.mat = mat;
		return true;
	}

private:
	point3 center;
	double radius;
	shared_ptr<material> mat;
};

#endif
