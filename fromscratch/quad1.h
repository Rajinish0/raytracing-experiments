#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"
#include "aabb.h"
#include "vec3.h"
#include "mat3.h"

#include <memory>



/*
Any point on the plane can be written as 
q + au + bv
in matrix from q + Ax
we need to solve  
q + Ax = r + dt 
where r is ray origin and d is its direction
Ax - dt = r - q
this is a 3x3 system of linear equation
[u v -d]x = r - q
where x = [a b t] 

Not optimized, the other quad2 is better
*/
//maybe plane is a better name
class quad : public hittable
{
public:
    quad(const point3& q, const vec3& u, const vec3& v, 
         std::shared_ptr<material> mat)
         :q(q), u(u), v(v), mat(mat)
    {
        normal = unit_vector(cross(u, v));
        set_bounding_box();
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override
    {
     /*solve the linear system of equations (3x3)
     there exits a soln. if inv(A) /= 0.
     */

     mat3 m(
        u, v, -r.direction()
     );

    //  double d = m.det();

    //  if (std::abs(d) < 1e-8) return false;
    bool solvable;
    vec3 p = m.solve(r.origin() - q, solvable);

    if (!solvable) return false;

    //  vec3 p = m.inv() * (r.origin() - q);
     double a = p[0],
            b = p[1],
            t = p[2];
         
    if (!ray_t.contains(t) || 
        !is_interior(a, b, rec)) return false;

     rec.p = r.at(t);
     rec.t = t;
     rec.mat = mat;
     rec.set_front_face(r, normal);
     return true;
    }

    virtual bool is_interior(double a, double b, hit_record& rec) const
    {
        if (a < 0. || a > 1. || 
            b < 0. || b > 1.) return false;

        rec.u = a;
        rec.v = b;

        return true;
    }

    virtual void set_bounding_box() {
        auto bbox_diag1 = aabb(q, q + u + v);
        auto bbox_diag2 = aabb(q + u, q + v);
        bbox = aabb(bbox_diag1, bbox_diag2);
        //bbox = aabb(q + (u + v), q - (u + v));
    }

    aabb bounding_box() const override
    {
        return bbox;
    }

private:
    /*
    q --> point on the quad (bottom left corner)
    u, v --> basis vectors, but not normalized
    i.e q + u should give the right edge
    so the width (or base?) is |u|
    */
    point3 q;
    vec3 u, v;
    vec3 normal;
    aabb bbox;
    std::shared_ptr<material> mat;
};

#endif