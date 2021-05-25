#ifndef _INTERSECT_H
#define _INTERSECT_H
#include "vec3.h"
#include "geometry.h"

class Geometry;
struct IntersectInfo
{
    float distance = NULL;
    vec3f position;
    vec3f normal;
    Geometry *geometry;
};
#endif