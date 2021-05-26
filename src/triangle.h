#ifndef _TRIANGLE_H
#define _TRIANGLE_H
#include "geometry.h"
#include "intersect-info.h"
#include "BVH/polygon.h"
#include "BVH/AABB.h"
#include "BSDF.h"
class Triangle : public Geometry
{
private:
    vec3f normal;
    Polygon *polygon;
    unsigned int FaceID;

public:
    Triangle(Polygon *poly, unsigned int ID)
    {
        polygon = poly;
        FaceID = ID;
        material = poly->material;
        bsdf = poly->bsdf;
        const auto index = polygon->getIndex(ID);
        vec3f v[3];
        v[0] = polygon->getVertex(index[0]);
        v[1] = polygon->getVertex(index[1]);
        v[2] = polygon->getVertex(index[2]);

        vec3f e1 = v[1] - v[0];
        vec3f e2 = v[2] - v[0];

        normal = closs(e1, e2);
        normal = normalize(normal);
    }

    bool hit(Ray &ray, IntersectInfo &info)
    {
        const auto index = polygon->getIndex(FaceID);
        vec3f v[3];
        v[0] = polygon->getVertex(index[0]);
        v[1] = polygon->getVertex(index[1]);
        v[2] = polygon->getVertex(index[2]);
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

        float f = dot(ray.getdirection(), beta) * invDet;
        if (f < 0.0f || u + f > 1.0f)
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

    AABB calcAABB()
    {
        const auto index = polygon->getIndex(FaceID);
        vec3f v0, v1, v2;
        v0 = polygon->getVertex(index[0]);
        v1 = polygon->getVertex(index[1]);
        v2 = polygon->getVertex(index[2]);

        vec3f pMin, pMax;
        for (int i = 0; i < 3; ++i)
        {
            pMin[i] = min(min(v0[i], v1[i]), v2[i]);
            pMax[i] = max(max(v0[i], v1[i]), v2[i]);
        }

        return AABB(pMin, pMax);
    }

    void faceCheck()
    {
        cout << FaceID << " :" << calcAABB().center() << endl;
    }
};
#endif