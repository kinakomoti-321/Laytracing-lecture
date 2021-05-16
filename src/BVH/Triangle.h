#ifndef _TRI_H_
#define _TRI_H_
#include "AABB.h"
#include "intersect-info.h"
#include "BVH/polygon.h"
#include "geometry.h"
#include <cmath>

class Triangle : public Geometry
{
private:
    const Polygon *polygon;
    unsigned int faceID;

public:
    Triangle(const Polygon *polygon, unsigned int faceID) : polygon(polygon), faceID(faceID)
    {
        basecolor = polygon->color;
        material = MaterialType::Emission;
    }

    unsigned int getFaceID() const
    {
        return faceID;
    }

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

    // bool intersect(const Ray &ray, IntersectInfo &info)
    // {
    //     vec3f v[3];
    //     const auto indices = polygon->getIndex(faceID);
    //     v[0] = polygon->getVertex(indices[0]);
    //     v[1] = polygon->getVertex(indices[1]);
    //     v[2] = polygon->getVertex(indices[2]);

    //     vec3f e1 = v[1] - v[0];
    //     vec3f e2 = v[2] - v[0];
    //     vec3f normal = closs(e1, e2);

    //     vec3f alpha = closs(ray.getdirection(), e2);
    //     float det = dot(e1, alpha);

    //     if (det == 0.0f)
    //         return false;

    //     float invDet = 1.0f / det;
    //     vec3f r = ray.getorigin() - v[0];

    //     float u = dot(alpha, r) * invDet;
    //     if (u < 0.0f || u > 1.0f)
    //     {
    //         return false;
    //     }

    //     vec3f beta = closs(r, e1);

    //     float w = dot(ray.getdirection(), beta) * invDet;
    //     if (w < 0.0f || u + w > 1.0f)
    //     {
    //         return false;
    //     }

    //     float t = dot(e2, beta) * invDet;
    //     if (t < ray.getkyoyou() || t > 10000)
    //     {
    //         return false;
    //     }

    //     info.position = ray.post(t);
    //     info.distance = t;
    //     info.geometry = this;
    //     if (dot(ray.getdirection(), normal) < 0)
    //     {
    //         info.normal = normal;
    //     }
    //     else
    //     {
    //         info.normal = -normal;
    //     }
    //     cout << "intersection" << endl;
    //     return true;
    // }
    using Vec3 = vec3f;
    bool intersect(const Ray &ray, IntersectInfo &info)
    {
        const auto indices = polygon->getIndex(faceID);
        const Vec3 v1 = polygon->getVertex(indices[0]);
        const Vec3 v2 = polygon->getVertex(indices[1]);
        const Vec3 v3 = polygon->getVertex(indices[2]);

        // https://www.tandfonline.com/doi/abs/10.1080/10867651.1997.10487468
        constexpr float EPS = 1e-8;
        const Vec3 e1 = v2 - v1;
        const Vec3 e2 = v3 - v1;

        const Vec3 pvec = closs(ray.getdirection(), e2);
        const float det = dot(e1, pvec);

        if (det > -EPS && det < EPS)
            return false;
        const float invDet = 1.0f / det;

        const Vec3 tvec = ray.getorigin() - v1;
        const float u = dot(tvec, pvec) * invDet;
        if (u < 0.0f || u > 1.0f)
            return false;

        const Vec3 qvec = closs(tvec, e1);
        const float v = dot(ray.getdirection(), qvec) * invDet;
        if (v < 0.0f || u + v > 1.0f)
            return false;

        const float t = dot(e2, qvec) * invDet;
        if (t < ray.tmin || t > ray.tmax)
            return false;

        info.distance = t;
        info.position = ray.post(t);

        // 面法線を計算
        vec3f normal = closs(e1, e2);
        info.normal = normalize(normal);

        info.geometry = this;
        return true;
    }
};
#endif