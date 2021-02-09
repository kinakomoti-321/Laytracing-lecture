#ifndef _PINCAMERA_H
#define _PINCAMERA_H
#include "vec3.h"
#include "ray.h"
using vec3f = vec3<float>;
class pincamera
{
private:
    vec3f camerapos;
    vec3f front;
    vec3f yoko;
    vec3f ue;
    vec3f pinhole;
    float width;
    float height;

public:
    pincamera(vec3f cp, vec3f f)
    {
        front = f;
        yoko = closs(f, vec3f(0, 1, 0));
        ue = closs(yoko, f);
        width = 2.0f;
        height = 2.0f;
        camerapos = cp;
        pinhole = cp + normalize(f);
    }

    ray getray(float i, float j)
    {
        vec3f imagepos = camerapos + i * yoko + j * ue;
        ray result(imagepos, pinhole);
        return result;
    }
};

#endif