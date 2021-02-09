#ifndef _INTERSECT_H
#define _INTERSECT_H
#include "vec3.h"
struct IntersectInfo
{
    float distance;
    vec3f position;
    vec3f normal;
    vec3f color;
};
#endif