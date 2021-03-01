#ifndef _VEC3_H
#define _VEC3_H
#include <iostream>
#include <cmath>
using namespace std;
template <typename T>
struct vec3
{
    T v[3];

    vec3() { v[0] = v[1] = v[2] = 0; }
    vec3(T x) { v[0] = v[1] = v[2] = x; }
    vec3(T x, T y, T z)
    {
        v[0] = x;
        v[1] = y;
        v[2] = z;
    }

    T operator[](unsigned int i) const { return v[i]; }
    T &operator[](unsigned int i) { return v[i]; }
    vec3 operator-() const { return vec3(-v[0], -v[1], -v[2]); }
    T norm() const
    {
        return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    }
    T norm2() const
    {
        return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    }
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
template <typename T>
inline vec3<T> operator*(const vec3<T> &a, const T &k)
{
    vec3<T> result;
    for (int i = 0; i < 3; ++i)
    {
        result[i] = a[i] * k;
    }
    return result;
}
template <typename T>
inline vec3<T> operator*(const T &k, const vec3<T> &a)
{
    vec3<T> result;
    for (int i = 0; i < 3; ++i)
    {
        result[i] = a[i] * k;
    }
    return result;
}
template <typename T>
inline vec3<T> operator/(const vec3<T> &a, const T &k)
{
    vec3<T> result;
    for (int i = 0; i < 3; ++i)
    {
        result[i] = a[i] / k;
    }
    return result;
}
template <typename T>
inline vec3<T> operator/(const T &k, const vec3<T> &a)
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

inline vec3f refrect(const vec3f &v, const vec3f &n)
{
    vec3f ref = v - 2 * dot(v, n) * n;
    return normalize(ref);
}
template <typename T>
inline bool operator==(vec3<T> a, vec3<T> b)
{
    return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
}
inline bool refraction(const vec3f &v, float n1, const vec3f &n, float n2, vec3f &r)
{
    const vec3f th = -(n1 / n2) * (v - dot(v, n) * n);
    if (th.norm2() > 1.0)
        return false;
    const vec3f tp = -sqrt(max(1.0f - th.norm2(), 0.0f)) * n;
    r = th + tp;
    return true;
}
#endif