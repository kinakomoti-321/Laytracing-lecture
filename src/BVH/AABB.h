#ifndef AABB_H
#define AABB_H
#include "intersect-info.h"
#include "vec3.h"
#include "ray.h"
#include <iostream>
#include <limits>
using namespace std;
struct AABB
{
public:
    vec3f bound[2];

    AABB()
    {
        bound[0] = vec3f(std::numeric_limits<float>::max());
        bound[1] = vec3f(std::numeric_limits<float>::min());
    }
    //コンストラクタ、minの方に一番下の頂点、maxの方に一番上の頂点
    AABB(vec3f dmin, vec3f dmax)
    {
        bound[0] = dmin;
        bound[1] = dmax;
    }

    vec3f center() const { return 0.5f * (bound[1] + bound[0]); }

    int longestAxis()
    {
        const vec3f length = bound[1] - bound[0];
        if (length[0] >= length[1] && length[0] >= length[2])
        {
            return 0;
        }
        else if (length[1] >= length[0] && length[1] >= length[2])
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }

    bool intersect(Ray &ray, const vec3f &dirInv, const int dirInvSign[3]) const
    {
        float tmin, tmax, tymin, tymax, tzmin, tzmax;
        tmin = (bound[dirInvSign[0]][0] - ray.origin[0]) * dirInv[0];
        tmax = (bound[1 - dirInvSign[0]][0] - ray.origin[0]) * dirInv[0];
        tymin = (bound[dirInvSign[1]][1] - ray.origin[1]) * dirInv[1];
        tymax = (bound[1 - dirInvSign[1]][1] - ray.origin[1]) * dirInv[1];
        if (tmin > tymax || tymin > tmax)
            return false;
        if (tymin > tmin)
            tmin = tymin;
        if (tymax < tmax)
            tmax = tymax;

        tzmin = (bound[dirInvSign[2]][2] - ray.origin[2]) * dirInv[2];
        tzmax = (bound[1 - dirInvSign[2]][2] - ray.origin[2]) * dirInv[2];
        if (tmin > tzmax || tzmin > tmax)
            return false;
        if (tzmin > tmin)
            tmin = tzmin;
        if (tzmax < tmax)
            tmax = tzmax;

        return tmin < ray.tmax && tmax > ray.tmin;
    }
};

inline AABB mergeAABB(const AABB &box, const vec3f pos)
{
    AABB res;
    for (int i = 0; i < 3; i++)
    {
        res.bound[0][i] = min(box.bound[0][i], pos[i]);
        res.bound[1][i] = max(box.bound[1][i], pos[i]);
    }

    return res;
}

inline AABB mergeAABB(const AABB &box1, const AABB &box2)
{
    AABB res;
    for (int i = 0; i < 3; i++)
    {
        res.bound[0][i] = min(box1.bound[0][i], box2.bound[0][i]);
        res.bound[1][i] = max(box1.bound[1][i], box2.bound[1][i]);
    }
    return res;
}

inline std::ostream &operator<<(std::ostream &stream, const AABB &bbox)
{
    stream << bbox.bound[0] << ", " << bbox.bound[1];
    return stream;
}

#endif