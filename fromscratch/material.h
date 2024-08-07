#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"

class material{
public:
	virtual ~material() = default;

	virtual bool scatter(
			const ray& r_in, const hit_record& rec, color& attenutaion, ray& scattered
			) const{
		return false;
	}
};

class lambertian : public material{
public:
	lambertian(const color& whiteness)
		:whiteness(whiteness) {}

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override{
		auto scatter_dir = rec.normal + random_unit_vector();
		if (scatter_dir.near_zero())
			scatter_dir = rec.normal;
		scattered = ray(rec.p, scatter_dir);
		attenuation = whiteness;
		return true;
	}

private:
	color whiteness;
};

class metal : public material{
public:
	metal(const color& whiteness, double fuzz)
		:whiteness(whiteness), fuzz(fuzz < 1 ? fuzz : 1){}

	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override{
		vec3 ref = reflect(r_in.direction(), rec.normal);
		ref = unit_vector(ref) + fuzz*random_unit_vector();
		scattered = ray(rec.p, ref);
		attenuation = whiteness;
		return (dot(scattered.direction(), rec.normal) > 0);
	}
private:
	color whiteness;
	double fuzz;
};


class dielectric : public material{
public:
	dielectric(double ior)
		:ior(ior){}
	bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {

		attenuation = color(1.0);
		//i think this only works if there's some seperation between two refractive objects
		//i.e the ray goes from air to this object and then back to air before going to another object
		double ri = rec.front_face ? (1.0/ior) : ior; 

		vec3 unit_direction = unit_vector(r_in.direction());
		double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
		double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

		vec3 direction;

		if (ri * sin_theta > 1.0 || reflectance(cos_theta, ri) > random_double() ){
			direction = reflect(unit_direction, rec.normal);
		}
		else{
			direction = refract(unit_direction, rec.normal, ri);
		}
		scattered = ray(rec.p, direction);
		return true;
	}

private:
	double ior;
	
	//SCHLICK
	static double reflectance(double cosine, double ri){
		auto r0 = (1-ri)/(1+ri);
		r0 *= r0;
		return r0 + (1-r0)*std::pow(1-cosine, 5);
	}
};


#endif
