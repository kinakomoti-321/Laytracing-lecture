#ifndef _SPHERE_H
#define _SPHERE_H
#include "vec3.h"
#include "ray.h"
#include "intersect-info.h"
#include "color.h"
#include <cmath>
#include <algorithm>
using namespace std;

enum class MaterialType
{
    Diffuse,
    Mirror,
    Glass
};

class Sphere
{
private:
    float radiance;
    vec3f position;
    vec3f basecolor;
    MaterialType material;

public:
    Sphere(float r, vec3f p) : radiance(r), position(p)
    {
        basecolor = vec3f(1, 1, 1);
        material = MaterialType::Diffuse;
    };

    Sphere(float r, vec3f p, vec3f c, MaterialType type) : radiance(r), position(p), basecolor(c), material(type){};

    bool hit(Ray &r, IntersectInfo &info)
    {
        float b = dot(r.getdirection(), r.getorigin() - position);
        vec3f alpha = r.getorigin() - position;
        float c = alpha.norm() * alpha.norm() - radiance * radiance;
        float D = b * b - c;

        if (D < 0)
            return false;

        float t1 = -b + sqrt(D);
        float t2 = -b - sqrt(D);

        float t = t2;
        if (t < 1e-3f || t > 10000)
        {
            t = t1;
            if (t < 1e-3f || t > 10000)
            {
                return false;
            }
        }

        info.distance = t;
        info.position = r.post(t);
        vec3f normal = info.position - position;
        info.normal = normalize(normal);
        info.sphere = this;
        return true;
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
#endif