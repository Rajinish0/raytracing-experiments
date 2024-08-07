#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "interval.h"

using color = vec3;

inline double linear_to_gamma(double linear_comp){
	if (linear_comp > 0)
		return std::sqrt(linear_comp);
	return 0;
}

void write_color(std::ostream& out, const color& pc){
	auto r = pc.x();
	auto g = pc.y();
	auto b = pc.z();

	r = linear_to_gamma(r);
	g = linear_to_gamma(g);
	b = linear_to_gamma(b);

	static const interval intensity(0.000, 0.999);
	int ir = (int)(256*intensity.clamp(r));
	int ig = (int)(256*intensity.clamp(g));
	int ib = (int)(256*intensity.clamp(b));

	out << ir << ' ' << ig << ' ' << ib << '\n';
}


#endif
