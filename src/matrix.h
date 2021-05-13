#ifndef MATRIX_H
#define MATRIX_H

#include "vec3.h"
#include <iostream>
class Matrix
{
public:
    float m[16];

    Matrix()
    {
        for (int i = 0; i < 16; i++)
        {
            m[i] = 0;
        }
    }

    float operator[](unsigned int i) const
    {
        return m[i];
    }
    float &operator[](unsigned int i)
    {
        return m[i];
    }
    //iが行,ｊが列
    float mq(unsigned int i, unsigned int j) const
    {
        return m[i * 4 + j];
    }
};

//平行移動行列
Matrix MoveMat(const vec3f &pos)
{
    Matrix mat;
    mat[0] = 1.0;
    mat[5] = 1.0;
    mat[10] = 1.0;
    mat[15] = 1.0;
    mat[3] = pos[0];
    mat[7] = pos[1];
    mat[11] = pos[2];

    return mat;
}

//拡大縮小行列
Matrix ScaleMat(const vec3f &scale)
{
    Matrix mat;
    mat[0] = scale[0];
    mat[5] = scale[1];
    mat[10] = scale[2];
    mat[15] = 1.0;
    return mat;
}

inline vec3f operator*(const Matrix &mat, const vec3f &pos)
{
    float b[4] = {pos[0], pos[1], pos[2], 1};
    float result[4];

    for (int i = 0; i < 4; i++)
    {
        float sum = 0;
        for (int j = 0; j < 4; j++)
        {
            sum += mat.mq(i, j) * b[j];
        }
        result[i] = sum;
    }

    return vec3f(result[0], result[1], result[2]);
}
inline Matrix operator*(Matrix &mat1, Matrix &mat2)
{
    Matrix result;

    return result;
}

inline std::ostream &operator<<(std::ostream &stream, const Matrix &mat)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            stream << mat[4 * i + j] << " ";
        }
        stream << endl;
    }

    return stream;
}

#endif