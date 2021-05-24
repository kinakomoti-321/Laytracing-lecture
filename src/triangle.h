#ifndef _TRIANGLE_H
#define _TRIANGLE_H
#include "geometry.h"
#include "intersect-info.h"
#include "BSDF.h"
class triangle : public Geometry
{
private:
    vec3f v[3];
    vec3f normal;

public:
    triangle(vec3f v0, vec3f v1, vec3f v2, vec3f color, MaterialType mater, BSDF *inbsdf)
    {
        v[0] = v0;
        v[1] = v1;
        v[2] = v2;

        vec3f e1 = v[1] - v[0];
        vec3f e2 = v[2] - v[0];

        normal = closs(e1, e2);
        normal = normalize(normal);
        material = mater;
        basecolor = color;
        geo = GeometryType::Triangle;
        bsdf = inbsdf;
    }

    bool hit(Ray &ray, IntersectInfo &info)
    {
        vec3f e1 = v[1] - v[0];
        vec3f e2 = v[2] - v[0];

        vec3f alpha = closs(ray.getdirection(), e2);
        float det = dot(e1, alpha);

        if (det == 0.0f)
            return false;

        float invDet = 1.0f / det;
        vec3f r = ray.getorigin() - v[0];

        float u = dot(alpha, r) * invDet;
        if (u < 0.0f || u > 1.0f)
        {
            return false;
        }

        vec3f beta = closs(r, e1);

        float v = dot(ray.getdirection(), beta) * invDet;
        if (v < 0.0f || u + v > 1.0f)
        {
            return false;
        }

        float t = dot(e2, beta) * invDet;
        if (t < ray.getkyoyou() || t > 10000)
        {
            return false;
        }

        info.position = ray.post(t);
        info.distance = t;
        info.geometry = this;

        if (dot(ray.getdirection(), normal) < 0)
        {
            info.normal = normal;
        }
        else
        {
            info.normal = -normal;
        }

        return true;
    }
};
#endif