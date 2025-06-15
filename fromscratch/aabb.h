#ifndef AABB_H
#define AABB_H

#include "interval.h"
#include "vec3.h"
#include "ray.h"

class aabb{
public:
    interval x, y, z;

    aabb() {}
    aabb(interval x, interval y, interval z) 
        :x(x), y(y), z(z){}

    aabb(const point3& a, const point3& b){
        x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
    } 
    aabb(const aabb& a, const aabb& b){
        x = interval(a.x, b.x);
        y = interval(a.y, b.y);
        z = interval(a.z, b.z);
    }

    const interval& axis_interval(int axis) const {
        if (axis == 1) return y;
        if (axis == 2) return z;
        return x;
    }

    bool hit(const ray& r, interval ray_t) const {
        const point3& r_orig = r.origin();
        const vec3&   r_dir  = r.direction();
        
        for (int axis =0; axis < 3; ++axis){
            const interval& axis_itvl = axis_interval(axis);
            const double dinv         = 1./r_dir[axis];

            auto t0 = (axis_itvl.min - r_orig[axis]) * dinv;
            auto t1 = (axis_itvl.max - r_orig[axis]) * dinv;

            if (t0 < t1){
                if (t0 > ray_t.min) ray_t.min = t0;
                if (t1 < ray_t.max) ray_t.max = t1;
            } else {
                if (t1 > ray_t.min) ray_t.min = t1;
                if (t0 < ray_t.max) ray_t.max = t0;
            }

            if (ray_t.max <= ray_t.min) return false;
        }

        return true;
    }
};

#endif