#include "quad.h"


class circle : public quad
{
public:
    //c is the center, u, v define the plane
    //u, v have to be orthogonal to each other
    //and their length has to equal to radius (can also draw an ellipse, this way)
    //I like how it is now but later I could do gramschmidt to get orthogonal v
    circle(const point3& q, const vec3& u, const vec3& v,
           std::shared_ptr<material> mat)
        :quad(q, u, v, mat), rad1( u.length() ), rad2( v.length() )
    {}
    
    virtual bool is_interior(double a, double b, hit_record& rec) const override
    {
        if (a * a + b * b > 1) return false;

        rec.u = 0.5 + 0.5 * a; // map from [-1,1] to [0,1]
        rec.v = 0.5 + 0.5 * b;
        return true;
    }

private:
    double rad1, rad2;
};