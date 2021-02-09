#ifndef _SPHERE_H
#define _SPHERE_H
#include "vec3.h"
#include "ray.h"
#include "intersect-info.h"
#include <cmath>
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
    MaterialType material;

public:
    Sphere(float r, vec3f p)
    {
        radiance = r;
        position = p;
    }

    bool hit(ray &r, IntersectInfo &info)
    {
        float b = dot(r.getdirection(), r.getorigin() - position);
        vec3f alpha = r.getorigin() - position;
        float c = alpha.norm() * alpha.norm() - radiance * radiance;
        float D = b * b - c;

        if (D < 0)
            return false;

        float t1 = -b + sqrt(D);
        float t2 = -b - sqrt(D);

        vec3f pos1 = r.post(t1);
        vec3f pos2 = r.post(t2);
        vec3f a1 = pos1 - r.getorigin();
        vec3f a2 = pos2 - r.getorigin();
        if (a1.norm() < a2.norm())
        {
            info.position = pos1;
            vec3f tekitou = pos1 - position;
            info.normal = normalize(tekitou);
            info.distance = a1.norm();
            return true;
        }

        else
        {
            info.position = pos2;
            vec3f tekitou = pos2 - position;
            info.normal = normalize(tekitou);
            info.distance = a2.norm();
            return true;
        }
    }
};
#endif