#ifndef _RAY_H
#define _RAY_H
#include "vec3.h"
#include <limits>
using vec3f = vec3<float>;
class Ray
{
private:
public:
    float tmax{std::numeric_limits<float>::max()};
    float tmin{1e-3f};
    vec3f origin;
    vec3f direction;
    float kyoyou = 1e-3f;
    Ray(vec3f o, vec3f d) : origin(o), direction(d)
    {
    }

    vec3f post(const float t) const
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
    float getkyoyou() const
    {
        return kyoyou;
    }
};

#endif