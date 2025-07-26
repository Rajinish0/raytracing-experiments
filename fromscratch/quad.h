// #define USE_EXPERIMENTAL_QUAD

#ifndef QUAD_MAIN_H
#define QUAD_MAIN_H

#ifdef USE_EXPERIMENTAL_QUAD
#include "quad1.h"
#else
#include "quad2.h"
#endif 


inline shared_ptr<hittable_list> box(const point3& a, const point3& b, shared_ptr<material> mat)
{
    auto sides = make_shared<hittable_list>();

    auto min = point3(std::fmin(a.x(), b.x()), 
                      std::fmin(a.y(), b.y()),
                      std::fmin(a.z(), b.z()));

    auto max = point3(std::fmax(a.x(), b.x()), 
                      std::fmax(a.y(), b.y()),
                      std::fmax(a.z(), b.z()));

    auto dx = vec3(max.x() - min.x(), 0., 0.);
    auto dy = vec3(0., max.y() - min.y(), 0.);
    auto dz = vec3(0., 0., max.z() - min.z());

    sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()),  dx, dy, mat)); //F
    sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz, dy, mat)); //R
    sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx, dy, mat)); //BACK
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dz, dy, mat)); //L
    sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()),  dx,-dz, mat)); //T
    sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()),  dx, dz, mat)); //BOTTOM

    return sides;
}

#endif