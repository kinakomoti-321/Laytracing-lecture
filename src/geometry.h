#ifndef _GEOMETRY_H
#define _GEOMETRY_H
#include "intersect-info.h"
#include "ray.h"
#include "vec3.h"
#include "rng.h"
#include <math.h>
#include "BSDF.h"
// 接空間での半球面一様サンプリング
// vec3f sampleHemisphere(float u, float v)
// {
//     const float theta = std::acos(clamp(1.0f - 2.0f * u, -1.0f, 1.0f)) / 2.0f;
//     const float phi = 2.0f * M_PI * v;
//     return vec3f(std::cos(phi) * std::sin(theta), std::cos(theta),
//                  std::sin(phi) * std::sin(theta));
// }

struct IntersectInfo;
enum class MaterialType
{
    Diffuse,
    Mirror,
    Glass,
    Emission
};

enum class GeometryType
{
    Sphere,
    Rectangle,
    Triangle
};

struct Material
{
    MaterialType material;
    vec3f color;
    float Luminescenece;
};
class Geometry
{
protected:
    GeometryType geo;
    vec3f position;
    vec3f basecolor;
    MaterialType material;

public:
    BSDF *bsdf;

    virtual bool hit(Ray &r, IntersectInfo &info)
    {
        return false;
    }

    MaterialType getMaterial() const
    {
        return material;
    }

    vec3f getColor() const
    {
        return basecolor;
    }
};

class Object
{
};
#endif