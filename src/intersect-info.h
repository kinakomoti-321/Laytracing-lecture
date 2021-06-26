#ifndef _INTERSECT_H
#define _INTERSECT_H
#include "vec3.h"
#include "geometry.h"
#include "BVH/polygon.h"
class Geometry;
struct IntersectInfo
{
    float distance = 10000.0;
    vec3f position;
    vec3f normal;
    float uv[2];
    Polygon *poly;
    Geometry *geometry;
};
#endif