#ifndef _RAY_H
#define _RAY_H
#include "vec3.h"
using vec3f = vec3<float>;
class ray
{
private:
    vec3f origin;
    vec3f position;
    vec3f direction;
    float kyoyou;

public:
    ray(vec3f o, vec3f p, float k)
    {
        origin = o;
        position = p;
        vec3f c = p - o;
        direction = normalize(c);
        kyoyou = k;
    }
    ray(vec3f o, vec3f p)
    {
        origin = o;
        position = p;
        vec3f c = p - o;
        direction = normalize(c);
        kyoyou = 0;
    }

    void inputdirection(vec3f d)
    {
        direction = d;
    }

    vec3f post(const float t)
    {
        return origin + t * direction;
    }

    void setkyoyou(const float k)
    {
        kyoyou = k;
    }

    vec3f getorigin() const
    {
        return origin;
    }
    vec3f getposition() const
    {
        return position;
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