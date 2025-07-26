#ifndef MAT3_H
#define MAT3_H

#include <istream>
#include <ostream>
#include "vec3.h"

#define EPSILON 1e-8

class mat3 {
private:
    double m[9];

public:
    using coord = std::tuple<int, int>;
    mat3(){
        fill(0.);
    }

    mat3(double v){
        fill(v);
    }

    mat3(vec3 v1, vec3 v2, vec3 v3){
        for (int i =0; i < 3; ++i){
            (*this) << v1[i] << v2[i] << v3[i];
        }
    }

    inline size_t idx(size_t i, size_t j) const
    {
        return (j + i * 3);
    }

    void fill(double val)
    {
        for (int i =0; i < 9; ++i)
            m[i] = val;
    }

    mat3 operator+ (const mat3& other) const
    {
        mat3 sum{};
        for (int i =0; i < 9; ++i)
            sum[i] = m[i] + other[i];

        return sum;
    }

    mat3 operator* (const mat3& other) const
    {
        mat3 prod{};
        for (int i =0; i < 3; ++i) 
        {
            for (int j =0; j < 3; ++j)
            {
                for (int k =0; k < 3; ++k)
                {
                    prod[idx(i, j)] += m[idx(i, k)]*other[idx(k, j)];
                }
            }
        }
        return prod;
    }

    double det() const
    {
        return at(0, 0)*(at(1, 1) * at(2, 2) - at(2, 1) * at(1, 2))
              -at(0, 1)*(at(1, 0) * at(2, 2) - at(1, 2) * at(2, 0))
              +at(0, 2)*(at(1, 0) * at(2, 1) - at(2, 0) * at(1, 1) );
    }

    double det_two_by_two(coord c1, coord c2, coord c3, coord c4) const 
    {
        auto &[i0, j0] = c1;
        auto &[i1, j1] = c2;
        auto &[i2, j2] = c3;
        auto &[i3, j3] = c4;

        return at(i0, j0)*at(i2, j2) - at(i1, j1)*at(i3, j3);
    }

    double determinant_at(size_t i, size_t j) const
    {
        bool inc_j = (j == 1);
        bool inc_i = (i == 1);

        size_t si = (i > 0) ? 0 : i+1;
        size_t sj = (j > 0) ? 0 : j+1;

        double determ = at(si, sj) * at(si + inc_i + 1, sj + inc_j + 1) -
                        at(si + 1 + inc_i, sj) * at(si, sj + 1 + inc_j);
        return determ;
    }

    mat3 inv() const {
        mat3 adjoint;
        for (int i =0; i < 3; ++i){
            for (int j =0; j < 3; ++j){
                //transpose
                adjoint.at(j, i) = determinant_at(i, j) * ((i + j)%2 == 0 ? 1 : -1);
            }
        }

        return adjoint / det();
    }

    //do gaussian elimiation
    vec3 solve(vec3 b, bool& solvable) const
    {

        vec3 res;
        size_t h = 0, k = 0;
        mat3 cp = (*this); // work on a copy
        
        #define EARLY_EXIT() do { solvable=false; return res; } while(0)

        //reduce to row echelon
        while (h < 3 && k < 3)
        {
            size_t i_max = h;
            for (int i = h + 1; i < 3; ++i)
                if (std::fabs(cp.at(i, k)) > std::fabs(cp.at(i_max, k))) i_max = i;

            if (std::fabs(cp.at(i_max, k)) < EPSILON) k++;
            else{
                cp.swap_rows(h, i_max);
                b.swap_rows(h, i_max);
                for (int i =h+1; i < 3; ++i)
                {
                    if (std::fabs(cp.at(h, k)) < EPSILON) EARLY_EXIT();
                    double f = cp.at(i, k)/cp.at(h, k);
                    cp.at(i, k) = 0;
                    for (int j = k + 1; j < 3; ++j)
                        cp.at(i, j) = cp.at(i, j) - cp.at(h, j) * f;
                    b[i] = b[i] - b[h] * f;
                }
                h++; k++;
            }
        }

        res = b;
        //do back subtitution (cp is now in upper triangular form)
        for (int i =2; i >= 0; i--)
        {
            if (std::fabs(cp.at(i, i)) < EPSILON) EARLY_EXIT();

            for (int j = 2; j >= i; j--)
            {
                if (i == j) res[i] /= cp.at(i, j);
                else res[i] -= cp.at(i, j) * res[j];
            }
        }

        solvable = true;
        return res;
    }

    [[nodiscard]]
    mat3 operator/(double v) const
    {
        mat3 out;
        for (int i =0; i < 9; ++i)
            out << (m[i] / v);
        return out;
    }

    mat3& operator/=(double v)
    {
        for (int i =0 ; i < 9; ++i)
            m[i] /= v;

        return *this;
    }

    mat3 operator-() const
    {
        mat3 mat;
        for (int i =0; i < 9; ++i)
            mat << -m[i];
        return mat;
    }

    [[nodiscard]]
    mat3 operator*(double v) const
    {
        mat3 out;
        for (int i =0; i < 9; ++i)
            out << m[i] * v;

        return out;
    }

    mat3& operator*=(double v)
    {
        for (int i =0; i < 9; ++i)
            m[i] *= v;
        return *this;
    }

    [[nodiscard]]
    vec3 operator*(vec3 vec) const
    {
        vec3 out;
        for (int i =0; i < 3; ++i)
            for (int j =0; j < 3; ++j)
                out[i] += at(i, j)*vec[j];

        return out;
    }

    double& operator[](size_t idx)
    {
        return m[idx];
    }

    const double& operator[](size_t idx) const
    {
        return m[idx];
    }

    double& at(size_t i, size_t j)
    {
        return m[idx(i, j)];
    }

    const double& at(size_t i, size_t j) const
    {
        return m[idx(i, j)];
    }

    mat3& operator<< (const double &v)
    {
        m[_cur_ind] = v;
        _cur_ind = (_cur_ind + 1) % 9;

        return (*this);
    }

    static mat3 rot_x(double theta){
        double c = std::cos(theta); 
        double s = std::sin(theta);

        mat3 m;
        
        m << 1 << 0 << 0 
          << 0 << c << -s 
          << 0 << s << c;

        return m;
    }

    static mat3 rot_y(double theta){
        double c = std::cos(theta); 
        double s = std::sin(theta);

        mat3 m;
        
        m << c << 0 << s 
          << 0 << 1 << 0 
          <<-s << 0 << c;

        return m;
    }

    static mat3 rot_z(double theta)
    {
        double c = std::cos(theta); 
        double s = std::sin(theta);

        mat3 m;
        
        m << c << -s << 0 
          << s <<  c << 0 
          << 0 <<  0 << 1;

        return m;
    }

    template<size_t axis>
    std::enable_if_t<axis <= 2, mat3>
    static rot(double theta)
    {
        switch (axis){
            case 0:  return rot_x(theta);
            case 1:  return rot_y(theta);
            default: return rot_z(theta);
        }
    }

private:
    size_t _cur_ind = 0;

    void swap_rows(const size_t r1, const size_t r2){
        double tmp;
        for (int i = 0; i < 3; ++i)
        {
            tmp = at(r1, i);
            this->at(r1, i) = this->at(r2, i);
            this->at(r2, i) = tmp;
        }
    }
};

#endif