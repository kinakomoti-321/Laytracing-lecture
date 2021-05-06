#ifndef _RAYTRACER_H
#define _RAYTRACER_H
#include "intersect-info.h"
#include "scene.h"
#include "anbient.h"
#include "rng.h"
#include <iostream>
using namespace std;

vec3f raytracing(Ray &r, Scene &scene, RNGrandom &rng)
{
    int Max_Depth = 100;
    vec3f lightpos(1, 3, 0);
    IntersectInfo info;
    Ray ray = r;
    float refractance[2] = {1.0f, 1.5f};
    for (int i = 1; i <= Max_Depth; ++i)
    {
        if (scene.hit(ray, info))
        {
            MaterialType type = info.geometry->getMaterial();
            if (type == MaterialType::Diffuse)
            {
                vec3f light = lightpos - info.position;
                float lightdis = light.norm2();
                light = normalize(light);
                Ray lightray(info.position, light);
                vec3f anb = info.geometry->getColor() * 0.2f;

                if (scene.hit(lightray, info))
                {
                    return anb;
                }
                else
                {
                    return max(dot(light, info.normal), 0.0f) * info.geometry->getColor() * 5.0f / lightdis + 0.2f * anb;
                }
            }
            else if (type == MaterialType::Glass)
            {
                bool is_inside = dot(-ray.getdirection(), info.normal) < 0;
                vec3f next_direction;
                if (!is_inside)
                {
                    if (!refraction(-ray.getdirection(), 1.0f, info.normal, 1.5f, next_direction))
                    {
                        next_direction = refrect(ray.getdirection(), info.normal);
                    }
                }
                else
                {
                    if (!refraction(-ray.getdirection(), 1.5f, -info.normal, 1.0f, next_direction))
                    {
                        next_direction = refrect(ray.getdirection(), -info.normal);
                    }
                }

                ray = Ray(info.position, next_direction);
            }
            else if (type == MaterialType::Mirror)
            {
                vec3f rdirection = refrect(ray.getdirection(), info.normal);
                ray = Ray(info.position, rdirection);
            }
        }
        else
        {
            return vec3f(0, 0, 0);
        }
    }

    return vec3f(0, 0, 0);
}

#endif