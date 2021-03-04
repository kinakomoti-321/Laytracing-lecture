#ifndef _SCENE_H
#define _SCENE_H
#include "sphere.h"
#include "ray.h"
#include "intersect-info.h"
#include "geometry.h"
#include "rectangle.h"
#include "triangle.h"
#include <vector>
class Scene
{
private:
    vector<Geometry *> geometry;

public:
    Scene(vec3f origin, float f)
    {
        Geometry *geo = new Sphere(f, origin);
        geometry.push_back(geo);
    }
    Scene(){};
    ~Scene()
    {
        //ジオメトリ配列のメモリを消す
        for (int i = 0; i < geometry.size(); ++i)
        {
            delete geometry[i];
        }
    }

    void SphereAdd(const vec3f &origin, const float &f)
    {
        Geometry *geo = new Sphere(f, origin);
        geometry.push_back(geo);
    }
    void SphereAdd(const vec3f &origin, const float &f, const vec3f &color, const MaterialType &type)
    {
        Geometry *geo = new Sphere(f, origin, color, type);
        geometry.push_back(geo);
    }
    void RectangleAdd(const vec3f &origin, const vec3f &normal, const vec3f &color, MaterialType mater)
    {
        Geometry *geo = new Rectangle(origin, normal, color, mater);
        geometry.push_back(geo);
    }

    void TriangleAdd(const vec3f &v0, const vec3f &v1, const vec3f &v2, const vec3f &color, const MaterialType mater)
    {
        Geometry *geo = new Triangle(v0, v1, v2, color, mater);
        geometry.push_back(geo);
    }

    void PyramidAdd(const vec3f vertex[4], const vec3f color, const MaterialType mater)
    {
        TriangleAdd(vertex[0], vertex[1], vertex[2], color, mater);
        TriangleAdd(vertex[0], vertex[1], vertex[3], color, mater);
        TriangleAdd(vertex[1], vertex[2], vertex[3], color, mater);
        TriangleAdd(vertex[0], vertex[2], vertex[3], color, mater);
    }

    bool hit(Ray &r, IntersectInfo &info)
    {
        float min = 10000;
        bool check = false;
        //各球に判定を行う
        for (int i = 0; i < geometry.size(); ++i)
        {
            IntersectInfo checkinfo;
            //一番近い点を取得する。
            if (geometry[i]->hit(r, checkinfo))
            {
                if (checkinfo.distance < min)
                {
                    min = checkinfo.distance;
                    info = checkinfo;
                    check = true;
                }
            }
        }
        return check;
    }
};
#endif