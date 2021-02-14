#ifndef _VEC3_H
#define _VEC3_H
#include <iostream>
#include <cmath>
using namespace std;
template <class T>
class vec3
{
private:
    T v[3];

public:
    vec3(T a, T b, T c)
    {
        v[0] = a;
        v[1] = b;
        v[2] = c;
    }
    vec3(T a)
    {
        v[0] = v[1] = v[2] = a;
    }
    vec3()
    {
        v[0] = v[1] = v[2] = 0;
    }

    const float norm()
    {
        return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    }

    T operator[](unsigned int i) const { return v[i]; }
    T &operator[](unsigned int i) { return v[i]; }
};

template <typename T>
inline vec3<T> operator+(const vec3<T> &a, const vec3<T> &b)
{
    vec3<T> result;
    for (int i = 0; i < 3; ++i)
    {
        result[i] = a[i] + b[i];
    }
    return result;
}
template <typename T>
inline vec3<T> operator-(const vec3<T> &a, const vec3<T> &b)
{
    vec3<T> result;
    for (int i = 0; i < 3; ++i)
    {
        result[i] = a[i] - b[i];
    }
    return result;
}
template <typename T, typename F>
inline vec3<T> operator*(const vec3<T> &a, const F &k)
{
    vec3<T> result;
    for (int i = 0; i < 3; ++i)
    {
        result[i] = a[i] * k;
    }
    return result;
}
template <typename T, typename F>
inline vec3<T> operator*(const F &k, const vec3<T> &a)
{
    vec3<T> result;
    for (int i = 0; i < 3; ++i)
    {
        result[i] = a[i] * k;
    }
    return result;
}
template <typename T, typename F>
inline vec3<T> operator/(const vec3<T> &a, const F &k)
{
    vec3<T> result;
    for (int i = 0; i < 3; ++i)
    {
        result[i] = a[i] / k;
    }
    return result;
}
template <typename T, typename F>
inline vec3<T> operator/(const F &k, const vec3<T> &a)
{
    vec3<T> result;
    for (int i = 0; i < 3; ++i)
    {
        result[i] = a[i] / k;
    }
    return result;
}

template <typename T>
inline vec3<T> normalize(vec3<T> &v)
{
    return v / v.norm();
}

template <typename T>
inline vec3<T> closs(const vec3<T> &a, const vec3<T> &b)
{
    vec3<T> result;
    result[0] = a[1] * b[2] - b[1] * a[2];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];

    return result;
}

template <typename T>
inline vec3<T> adamarl(const vec3<T> &a, const vec3<T> &b)
{
    vec3<T> result;
    for (int i = 0; i < 3; ++i)
    {
        result[i] = a[i] * b[i];
    }
    return result;
}

template <typename T>
inline T dot(const vec3<T> &a, const vec3<T> &b)
{
    T result = 0;
    for (int i = 0; i < 3; ++i)
    {
        result += a[i] * b[i];
    }
    return result;
}

template <typename T>
std::ostream &operator<<(std::ostream &stream, const vec3<T> &v)
{
    stream << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
    return stream;
}
using vec3f = vec3<float>;

template <typename T>
inline vec3<T> refrect(vec3<T> &v, vec3<T> &n)
{
    vec3<T> ref;
    ref = -1 * v + 2 * dot(v, n) * n;
    return normalize(ref);
}

inline vec3f refraction(vec3f &v, float n1, vec3f &n, float n2)
{
    vec3f th = -(n1 / n2) * (v - dot(v, n) * n);
    float b = 1 - th.norm() * th.norm();
    if (b > 0)
    {
        return -sqrt(b) * n + th;
    }
    return refrect(v, n);
}
#endif