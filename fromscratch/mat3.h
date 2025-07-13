#ifndef MAT3_H
#define MAT3_H

#include <istream>
#include <ostream>
#include "vec3.h"

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

private:
    size_t _cur_ind = 0;

};


#endif