#ifndef VEC3_H
#define VEC3_H


class vec3 {
public:	
	double e[3];

	vec3()
	  :e{0, 0, 0} {}

	vec3(double val)
	  :e{val, val, val} {}

	vec3(double x, double y, double z)
	  :e{x, y, z} {}
	
	double x() const {return e[0];}
	double y() const {return e[1];}
	double z() const {return e[2];}

	vec3 operator-() const {return vec3(-e[0], -e[1], -e[2]); }
	double operator[](int i) const {return e[i];}
	double& operator[](int i) {return e[i];} // to be able to modify
	
	vec3& operator+=(const vec3& v){
		for (int i =0; i < 3; i++)
			e[i] += v.e[i];
		return (*this);
	}

	vec3& operator*=(double t){
		for (int i =0; i < 3; i++)
			e[i] *= t;
		return (*this);
	}

	vec3& operator/=(double t){
		for (int i =0; i < 3; i++)
			e[i] /= t;
		return (*this);
	}

	double length() const {
		return std::sqrt(length_squared());
	}

	double length_squared() const{
		double s = 0;
		for (int i =0; i < 3; i++)
			s += e[i]*e[i];
		return s;
	}

	bool near_zero() const{
		return (std::fabs(e[0] < EPS_F) && 
				std::fabs(e[1] < EPS_F) &&
				std::fabs(e[2] < EPS_F));
	}

	static vec3 random(){
		return vec3(random_double(), random_double(), random_double());
	}
	
	static vec3 random(double min, double max){
		return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}
};

using point3 = vec3;

inline std::ostream& operator<<(std::ostream& out, const vec3& v){
	for (int i =0; i < 3; i++)
		out << v.e[i] << ' ';
	return out;
}

inline std::istream& operator>>(std::istream& in, vec3& v){
	for (int i =0; i < 3; i++)
		in >> v.e[i];
	return in;
}

inline vec3 operator+(const vec3& u, const vec3& v){
	return vec3(
			u.e[0] + v.e[0],
			u.e[1] + v.e[1],
			u.e[2] + v.e[2]
			);
}

inline vec3 operator-(const vec3& u, const vec3& v){
	return vec3(
			u.e[0] - v.e[0],
			u.e[1] - v.e[1],
			u.e[2] - v.e[2]
			);
}

inline vec3 operator*(const vec3& u, const vec3& v){
	return vec3(
			u.e[0] * v.e[0],
			u.e[1] * v.e[1],
			u.e[2] * v.e[2]
			);
}

inline vec3 operator*(double t, const vec3& v){
	return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3& v, double t){
	return t * v;
}

inline vec3 operator/(const vec3& v, double t){
	return (1/t) * v;
}

inline double dot(const vec3& u, const vec3& v){
	double sum = 0.0;
	for (int i =0; i < 3; i++)
		sum += u.e[i] * v.e[i];
	return sum;
}

inline vec3 cross(const vec3& u, const vec3& v){
	return vec3(
			u.e[1]*v.e[2] - u.e[2]*v.e[1],
			u.e[2]*v.e[0] - u.e[0]*v.e[2],
			u.e[0]*v.e[1] - u.e[1]*v.e[0]
			);
}

inline vec3 unit_vector(const vec3& v){
	return v / v.length();
}

inline vec3 random_in_unit_disk(){
	while (true){
		auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
		if (p.length_squared() < 1)
			return p;
	}
}

inline vec3 random_in_unit_sphere(){
	while (true){
		auto p = vec3::random(-1, 1);
		if (p.length_squared() < 1)
			return p;
	}
}
/*
inline vec3 random_unit_vector(){
	return unit_vector(random_in_unit_sphere());
}
*/


inline vec3 random_unit_vector(){
	auto theta = random_double(0.0, 2.0 * pi);
	auto phi   = random_double(-pi/2.0, pi/2.0);

	return vec3(
			 std::cos(theta) * std::cos(phi),
			 std::sin(phi),
			 std::sin(theta) * std::cos(phi)
			);
}


inline vec3 random_on_hemisphere(const vec3& normal){
	vec3 v = random_unit_vector();
	if (dot(v, normal) > 0)
		return v;
	return -v;
}

//n is in the opposite direction to v
inline vec3 reflect(const vec3& v, const vec3& n){
	return v - 2*dot(v, n)*n;
}


//V MUST BE NORMALIZED!!!!!
//the equation is just easier to derive when v is normalized
//if v is normalized then it can be broken into two components
//vorth + vparrallel (corresponding to n)
//vorth is just v + n*cos(theta) 
//the magnitude of this is sin(theta)
//v'orth needs to be in this direction but with the magitude of
//sin(theta') which is (eta/eta')*sin(theta)
//so multiplying vorth with (eta/eta') gives v'orth
//v'parallel is easy to obtain using pythagorus (the formula not the old man himself)
inline vec3 refract(const vec3& v, const vec3& n, double eta_over_etaprime){
	auto cos_theta = std::fmin(dot(-v, n), 1.0);
	vec3 r_out_orth = eta_over_etaprime*(v + cos_theta*n);
	vec3 r_out_par = -std::sqrt(std::fabs(1.0 - r_out_orth.length_squared()))*n;
	return r_out_par + r_out_orth;
}

#endif
