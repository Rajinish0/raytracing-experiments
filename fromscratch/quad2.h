#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"
#include "aabb.h"
#include "vec3.h"

#include <memory>
#include <cmath>


//maybe plane is a better name
class quad : public hittable
{
public:
    quad(const point3& q, const vec3& u, const vec3& v, 
         std::shared_ptr<material> mat)
         :q(q), u(u), v(v), mat(mat)
    {
        //assumes u, v are not parallel!
        vec3 n = cross(u, v); 
        normal = unit_vector(n);
        w = n/dot(n, n);
        D = dot(normal, q);
        set_bounding_box();
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override
    {
     double denom = dot(normal, r.direction());
     if (std::fabs(denom) < 1e-8) return false;
    
     double t = (D - dot(normal, r.origin()))/denom;

     if (!ray_t.contains(t)) return false;
    
     vec3 intersection = r.at(t);

     vec3 p = intersection - q;
     double alpha = dot(w, cross(p, v));
     double beta  = dot(w, cross(u, p));

    //  if (alpha < 0 || alpha > 1 || 
    //      beta  < 0 || beta  > 1) return false;

    if (!is_interior(alpha, beta, rec)) return false;

     rec.p = intersection;
     rec.t = t;
     rec.mat = mat;
     rec.set_front_face(r, normal);

     return true;
    }

    virtual bool is_interior(double a, double b, hit_record& rec) const
    {
        interval unit_itvl = interval(0, 1);

        if (!unit_itvl.contains(a) || !unit_itvl.contains(b)) return false;

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
    aabb bbox;
    std::shared_ptr<material> mat;
    vec3 normal, w;
    double D;
};

#endif