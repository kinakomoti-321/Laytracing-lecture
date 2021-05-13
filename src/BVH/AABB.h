#ifndef _AABB_H
#define _AABB_H

#include "ray.h"
#include "vec3.h"

#include <algorithm>
#include <iostream>
#include <limits>

struct AABB
{
    vec3f bounds[2];
    // explicit暗黙的な型変換を防ぐ
    explicit AABB() : bounds{vec3f(std::numeric_limits<float>::max()), vec3f(std::numeric_limits<float>::min())} {}

    explicit AABB(const vec3f &pMin, const vec3f &pMax) : bounds{pMin, pMax} {}

    vec3f center() const { return 0.5f * (bounds[0] + bounds[1]); }

    //一番長い軸を返す0はｘ1はy2はz
    int longestAxis() const
    {
        const vec3f length = bounds[1] - bounds[0];
        if (length[0] >= length[1] && length[0] >= length[2])
        {
            return 0;
        }
        else if (length[1] >= length[2] && length[1] >= length[0])
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }

    //高速なAABBの衝突判定　これにはレイの長さの逆数とそれぞれの符号が必要
    bool intersect(const Ray &ray, const vec3f &dirInv, const int dirInvSign[3]) const
    {
        float tmin, tmax, tymin, tymax, tzmin, tzmax;

        tmin = (bounds[dirInvSign[0]][0] - ray.getorigin()[0]) * dirInv[0];
        tmax = (bounds[1 - dirInvSign[0]][0] - ray.getorigin()[0]) * dirInv[0];
        tymin = (bounds[dirInvSign[1]][1] - ray.getorigin()[1]) * dirInv[1];
        tymax = (bounds[1 - dirInvSign[1]][1] - ray.getorigin()[1]) * dirInv[1];
        if (tmin > tymax || tymin > tmax)
            return false;
        if (tymin > tmin)
            tmin = tymin;
        if (tymax < tmax)
            tmax = tymax;

        tzmin = (bounds[dirInvSign[2]][2] - ray.getorigin()[2]) * dirInv[2];
        tzmax = (bounds[1 - dirInvSign[2]][2] - ray.getorigin()[2]) * dirInv[2];
        if (tmin > tzmax || tzmin > tmax)
            return false;
        if (tzmin > tmin)
            tmin = tzmin;
        if (tzmax < tmax)
            tmax = tzmax;

        return tmin < ray.tmax && tmax > ray.tmin;
    }
};

inline AABB mergeAABB(const AABB &bbox, const vec3f &p)
{
    AABB ret;
    for (int i = 0; i < 3; ++i)
    {
        ret.bounds[0][i] = std::min(bbox.bounds[0][i], p[i]);
        ret.bounds[1][i] = std::max(bbox.bounds[1][i], p[i]);
    }
    return ret;
}

inline AABB mergeAABB(const AABB &bbox1, const AABB &bbox2)
{
    AABB ret;
    for (int i = 0; i < 3; ++i)
    {
        ret.bounds[0][i] = std::min(bbox1.bounds[0][i], bbox2.bounds[0][i]);
        ret.bounds[1][i] = std::max(bbox1.bounds[1][i], bbox2.bounds[1][i]);
    }
    return ret;
}

#endif