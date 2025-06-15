#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "aabb.h"

class sphere : public hittable {
public:
	sphere(const point3& center, double radius, shared_ptr<material> mat)
		:center(center, vec3(0.)), radius(std::fmax(0, radius)), mat(mat){
			auto rd_vec = vec3(radius);
			bbox = aabb(center - rd_vec, center + rd_vec);	
		}

	sphere(const point3& c1, const point3& c2, double radius, shared_ptr<material> mat)
		:center(c1, c2 - c1), radius(std::fmax(0., radius)), mat(mat){
			auto rd_vec = vec3(radius);
			aabb bbox1 = aabb(c1 - rd_vec, c1 + rd_vec);
			aabb bbox2 = aabb(c2 - rd_vec, c2 + rd_vec);
			bbox = aabb(bbox1, bbox2);
		}


	bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
		point3 current_center = center.at(r.time());
		vec3 q = current_center - r.origin();
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
		vec3 o_normal = (rec.p - current_center)/radius;
		rec.set_front_face(r, o_normal);
		rec.mat = mat;
		get_sphere_uv(o_normal, rec.u, rec.v);

		return true;
	}

	aabb bounding_box() const override
	{
		return bbox;
	}

private:
	ray center;
	double radius;
	shared_ptr<material> mat;
	aabb bbox;


	static void get_sphere_uv(const point3& p, double& u, double& v)
	{
		auto theta = std::acos(-p.y());
		auto phi   = std::atan2(-p.z(), p.x()) + pi;

		u = phi/(2 * pi);
		v = theta / pi;
	}
};

#endif
