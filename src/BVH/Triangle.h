#ifndef _TRI_H_
#define _TRI_H_
#include "AABB.h"
#include "intersect-info.h"
#include "BVH/polygon.h"

class Triangle
{
private:
    const Polygon *polygon;
    unsigned int faceID;

public:
    Triangle(const Polygon *polygon, unsigned int faceID) : polygon(polygon), faceID(faceID) {}

    //三角形を取り囲むようなAABBを返す
    AABB calcAABB() const
    {
        const auto indices = polygon->getIndex(faceID);
        const vec3f v1 = polygon->getVertex(indices[0]);
        const vec3f v2 = polygon->getVertex(indices[1]);
        const vec3f v3 = polygon->getVertex(indices[2]);

        vec3f pMin, pMax;
        for (int i = 0; i < 3; ++i)
        {
            pMin[i] = std::min(std::min(v1[i], v2[i]), v3[i]);
            pMin[i] = std::max(std::max(v1[i], v2[i]), v3[i]);
        }

        return AABB(pMin, pMax);
    }

    bool intersect(const Ray &ray, IntersectInfo &info) const
    {
        vec3f v[3];
        const auto indices = polygon->getIndex(faceID);
        v[0] = polygon->getVertex(indices[0]);
        v[1] = polygon->getVertex(indices[1]);
        v[2] = polygon->getVertex(indices[2]);

        vec3f e1 = v[1] - v[0];
        vec3f e2 = v[2] - v[0];
        vec3f normal = closs(e1, e2);

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

        float w = dot(ray.getdirection(), beta) * invDet;
        if (w < 0.0f || u + w > 1.0f)
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