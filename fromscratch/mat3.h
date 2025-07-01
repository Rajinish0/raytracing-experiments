#ifndef MAT3_H
#define MAT3_H

#include <istream>
#include <ostream>

class mat3 {
private:
    double m[9];

public:
    mat3(){
        fill(0.);
    }

    inline size_t idx(size_t i, size_t j) const
    {
        return (j + i * 3);
    }

    void fill(double val){
        for (int i =0; i < 9; ++i)
            m[i] = val;
    }

    mat3 operator+ (const mat3& other){
        mat3 sum{};
        for (int i =0; i < 9; ++i)
            sum[i] = m[i] + other[i];

        return sum;
    }

    mat3 operator* (const mat3& other)
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

    double det()
    {
        return at(0, 0)*(at(1, 1) * at(2, 2) - at(2, 1) * at(1, 2))
              -at(0, 1)*(at(1, 0) * at(2, 2) - at(1, 2) * at(2, 0))
              +at(0, 2)*(at(1, 0) * at(2, 1) - at(2, 0) * at(1, 1) );
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

    std::istream& operator>> (std::istream& is, double v)
    {
        m[_cur_ind] = v;
        _cur_ind = (++_cur_ind) % 9;
    }

private:
    size_t _cur_ind = 0;

};


#endif