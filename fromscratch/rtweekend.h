#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>

#define EPS_F 1e-8

using std::make_shared;
using std::shared_ptr;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double deg_to_rad(double deg){
	return deg * pi / 180.0;
}

inline double random_double(){
	return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max){
	return min + (max-min)*random_double();
}

inline int random_int(int min, int max){
	return (int)random_double(min, max + 1);
}

template <typename T>
T lerp(T a, T b, float p)
{
	return a + (b-a)*p;
}

#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"

#endif
