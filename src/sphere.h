#ifndef _SPHERE_H
#define _SPHERE_H
#include "vec3.h"
#include "ray.h"
#include "intersect-info.h"
#include "color.h"
#include <cmath>
#include <algorithm>
using namespace std;

enum class MaterialType
{
    Diffuse,
    Mirror,
    Glass
};

class Sphere
{
private:
    float radiance;
    vec3f position;
    vec3f basecolor;
    MaterialType material;

public:
    Sphere(float r, vec3f p) : radiance(r), position(p)
    {
        basecolor = vec3f(1, 1, 1);
        material = MaterialType::Diffuse;
    };

    Sphere(float r, vec3f p, vec3f c, MaterialType type) : radiance(r), position(p), basecolor(c), material(type){};

    bool hit(Ray &r, IntersectInfo &info, vec3f &light)
    {
        float b = dot(r.getdirection(), r.getorigin() - position);
        vec3f alpha = r.getorigin() - position;
        float c = alpha.norm() * alpha.norm() - radiance * radiance;
        float D = b * b - c;

        if (D < 0)
            return false;

        float t1 = -b + sqrt(D);
        float t2 = -b - sqrt(D);

        vec3f pos1 = r.post(t1);
        vec3f pos2 = r.post(t2);
        vec3f a1 = pos1 - r.getorigin();
        vec3f a2 = pos2 - r.getorigin();
        float dis1 = a1.norm();
        float dis2 = a2.norm();

        if (dis1 < dis2 && dis1 > r.getkyoyou())
        {
            info.distance = dis1;
            vec3f rad = pos1 - position;
            info.normal = normalize(rad);
            info.position = pos1;
            vec3f getdirect = r.getdirection();
            info.color = this->Color(getdirect, info, light);
            return true;
        }
        else if (dis1 > dis2 && dis2 > r.getkyoyou())
        {
            info.distance = dis2;
            vec3f rad = pos2 - position;
            info.normal = normalize(rad);
            info.position = pos2;
            vec3f getdirect = r.getdirection();
            info.color = this->Color(getdirect, info, light);
            return true;
        }
        else
        {
            info.color = vec3f(0);
            return false;
        }
    }

    vec3f Color(vec3f &direction, IntersectInfo &info, vec3f &light)
    {
        switch (material)
        {
        case MaterialType::Diffuse:
            vec3f result;
            result = max(dot(info.normal, light), 0.0f) * basecolor;
            return result;
        case MaterialType::Mirror:
            return vec3f(0);

        case MaterialType::Glass:
            return vec3f(0);

        default:
            return vec3f(0);
        }
    }
};
#endif