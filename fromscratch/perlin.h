#ifndef PERLIN_H
#define PERLIN_H

#include "rtweekend.h"

class perlin {
public:
    perlin() {
        for (int i =0; i < point_count; ++i)
            randvec3[i] = unit_vector(vec3::random(-1, 1));
            // randdouble[i] = random_double();

        perlin_generate_perm(perm_x);
        perlin_generate_perm(perm_y);
        perlin_generate_perm(perm_z);
    }

    double noise(const point3& p) const 
    {
        // auto i = int(4*p.x()) & 255;
        // auto j = int(4*p.y()) & 255;
        // auto k = int(4*p.z()) & 255;

        // return randdouble[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
        static constexpr auto cnvt_to_int = [](double val){
            return val - std::floor(val);
        };

        auto u = cnvt_to_int(p.x());
        auto v = cnvt_to_int(p.y());
        auto w = cnvt_to_int(p.z());

        auto i = (int)std::floor(p.x());
        auto j = (int)std::floor(p.y());
        auto k = (int)std::floor(p.z());

        vec3 c[2][2][2];

        for (int di =0; di < 2; di++){
            for (int dj =0; dj < 2; dj++){
                for (int dk =0; dk < 2; dk++){
                    c[di][dj][dk] = randvec3[
                        perm_x[(i+di)&255] ^
                        perm_y[(j+dj)&255] ^
                        perm_z[(k+dk)&255]
                    ];
                }
            }
        }

        return trilinear_interp(c, u, v, w);

    }

    double fbm(const point3& p, int depth) const
    {
        auto accum = 0.;
        auto freq = 1.;
        auto amp = 1.;

        for (int i =0 ; i < depth; ++i){
            accum += amp * noise(freq * p);
            freq *= 2.;
            amp *= .5;
        }

        return std::fabs(accum);
    }

private:
    static const int point_count = 256;
    // double randdouble[point_count];
    vec3 randvec3[point_count];
    int perm_x[point_count],
        perm_y[point_count],
        perm_z[point_count];
    
    static void perlin_generate_perm(int *p)
    {
        for (int i =0; i < point_count; ++i)
            p[i] = i;
        
        permute(p, point_count);
    }

    static void permute(int *p, int n)
    {
        for (int i =n-1; i >0; i--)
        {
            int target = random_int(0, i);
            int tmp  = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
    }

    static double trilinear_interp(vec3 c[2][2][2], double u, double v, double w)
    {

        auto uu = u*u*(3-2*u);
        auto vv = v*v*(3-2*v);
        auto ww = w*w*(3-2*w);

        auto accum = 0.;
        for (int i =0; i < 2; i++)
            for (int j =0; j < 2; j++)
                for (int k =0; k < 2; k++){
                    //the vector to the edge
                    vec3 weight_v(u-i, v-j, w-k);
                    accum += (i*uu + (1-i)*(1-uu))
                           * (j*vv + (1-j)*(1-vv))
                           * (k*ww + (1-k)*(1-ww))
                           * dot(c[i][j][k], weight_v);
                }
        return accum;
    }
};

#endif