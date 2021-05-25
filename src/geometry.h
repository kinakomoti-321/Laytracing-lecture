#ifndef _GEOMETRY_H
#define _GEOMETRY_H
#include "intersect-info.h"
#include "ray.h"
#include "vec3.h"
#include "rng.h"
#include <math.h>
#include "BSDF.h"

struct IntersectInfo;
enum class MaterialType
{
    Diffuse,
    Mirror,
    Glass,
    Emission
};

class Geometry
{
protected:
    vec3f position;
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
};

class Object
{
};
#endif