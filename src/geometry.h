#ifndef _GEOMETRY_H
#define _GEOMETRY_H
#include "intersect-info.h"
#include "ray.h"
#include "vec3.h"
#include <math.h>

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
class Geometry
{
protected:
    GeometryType geo;
    vec3f position;
    vec3f basecolor;
    MaterialType material;

public:
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

    vec3f BSDF()
    {
        return basecolor / (float)M_PI;
    }
};
#endif