#ifndef _RECTANGLE_H
#define _RECTANGLE_H
#include "geometry.h"
#include "ray.h"

class Rectangle : public Geometry
{
private:
    vec3f normal;
    float x, y;

public:
    Rectangle(vec3f pos, vec3f n, vec3f color, MaterialType mater)
    {
        position = pos;
        normal = n;
        basecolor = color;
        material = mater;
    }

    bool hit(Ray &ray, IntersectInfo &info)
    {
        vec3f s = ray.getorigin() - position;
        vec3f d = ray.getdirection();

        float bunshi = dot(normal, d);
        if (bunshi == 0.0f)
        {
            return false;
        }

        float bunbo = dot(normal, s);
        float t = -bunbo / bunshi;

        if (t < ray.getkyoyou() || t > 10000)
            return false;
        info.position = ray.post(t);
        info.distance = t;
        info.geometry = this;

        if (bunshi > 0)
        {
            info.normal = -normal;
        }
        else
        {
            info.normal = normal;
        }

        return true;
    }
};

#endif