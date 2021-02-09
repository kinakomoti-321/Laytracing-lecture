#ifndef _SCENE_H
#define _SCENE_H
#include "sphere.h"
#include "ray.h"
#include "intersect-info.h"
#include <vector>
class Scene
{
private:
    vector<Sphere> sphere;

public:
    Scene(vec3f origin, float f)
    {
        Sphere first(f, origin);
        sphere.push_back(first);
    }

    void SphereAdd(vec3f origin, float f)
    {
        Sphere a(f, origin);
        sphere.push_back(a);
    }

    bool hit(ray &r, IntersectInfo &info)
    {
        float min = 10000;
        bool check = false;
        //各球に判定を行う
        for (int i = 0; i < sphere.size(); ++i)
        {
            IntersectInfo checkinfo;
            //一番近い点を取得する。
            if (sphere[i].hit(r, checkinfo))
            {
                if (checkinfo.distance < min)
                {
                    min = checkinfo.distance;
                    info = checkinfo;
                    check = true;
                }
            }
        }

        if (check)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};
#endif