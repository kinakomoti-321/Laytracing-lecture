#ifndef _RAY_H
#define _RAY_H
#include "vec3.h"
using vec3f = vec3<float>;
class Ray
{
private:
    vec3f origin;
    vec3f direction;
    float kyoyou = 1e-3f;

public:
    Ray(vec3f o, vec3f d) : origin(o), direction(d)
    {
    }

    vec3f post(const float t)
    {
        return origin + t * direction;
    }

    vec3f getorigin() const
    {
        return origin;
    }
    vec3f getdirection() const
    {
        return direction;
    }
    float getkyoyou()
    {
        return kyoyou;
    }
};

#endif