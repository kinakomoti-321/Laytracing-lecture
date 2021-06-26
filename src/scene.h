#ifndef _SCENE_H
#define _SCENE_H
#include "sphere.h"
#include "ray.h"
#include "intersect-info.h"
#include "geometry.h"
#include "rectangle.h"
#include "triangle.h"
#include "BVH/polygon.h"
#include <vector>
#include <cmath>
#include "BSDF.h"
#include "BVH/simpleBVH.h"
#include "Picture/Texture.h"

using namespace std;

class WorldSphere
{
private:
    float radiance = 10000;
    WorldTexture *tex = nullptr;

public:
    WorldSphere(WorldTexture *intex)
    {
        tex = intex;
    }

    vec3f hit(Ray &r)
    {
        // cout << "check" << endl;
        float b = dot(r.getdirection(), r.getorigin());
        vec3f alpha = r.getorigin();
        float c = alpha.norm() * alpha.norm() - radiance * radiance;
        float D = b * b - c;

        if (D < 0)
            return false;

        float t1 = -b + sqrt(D);
        float t2 = -b - sqrt(D);

        float t = t2;
        if (t < 1e-3f)
        {
            t = t1;
            if (t < 1e-3f)
            {
                return false;
            }
        }

        vec3f position = r.post(t);
        vec3f pos = normalize(position);
        float theta = acos(pos[1]);
        float phi = acos(pos[0] / sqrt(pos[0] * pos[0] + pos[2] * pos[2]));
        if (pos[2] < 0.0)
        {
            phi *= -1;
        }

        float u = theta / M_PI;
        float v = (phi + M_PI) / (2 * M_PI);
        v = 1 - v;
        if (u > 1.0 || u < 0.0)
            cout << u << " : " << v << endl;
        return tex->getTex(v, u);
        // return vec3f(u, v, 0.0);
    }
};

class Scene
{
private:
    vector<Geometry *> geometry;
    SimpleBVH BVH;
    WorldSphere *worldsphere = nullptr;

public:
    Scene(Polygon &polygon) : BVH(SimpleBVH(polygon))
    {
    }
    // Scene() {}

    ~Scene()
    {
        //ジオメトリ配列のメモリを消す
        for (int i = 0; i < geometry.size(); ++i)
        {
            delete geometry[i];
        }
    }

    void Build()
    {
        BVH.buildBVH();
    }

    void worldSet(WorldSphere *world)
    {
        worldsphere = world;
    }

    void SphereAdd(const vec3f &origin, const float &f)
    {
        Geometry *geo = new Sphere(f, origin);
        geometry.push_back(geo);
    }
    void SphereAdd(const vec3f &origin, const float &f, const MaterialType &type, BSDF *inbsdf)
    {
        Geometry *geo = new Sphere(f, origin, type, inbsdf);
        geometry.push_back(geo);
    }
    void RectangleAdd(const vec3f &origin, const vec3f &normal, MaterialType mater, BSDF *inbsdf)
    {
        Geometry *geo = new Rectangle(origin, normal, mater, inbsdf);
        geometry.push_back(geo);
    }

    void addPolygon(Polygon *polygon)
    {
        for (int i = 0; i < polygon->nFaces(); i++)
        {
            Geometry *geo = new Triangle(polygon, i);
            geometry.push_back(geo);
        }
    }

    void addPolygonBVH(Polygon &polygon)
    {
        BVH.AddPolygon(polygon);
    }
    bool hit(Ray &r, IntersectInfo &info)
    {
        bool check = false;
        //BVH側の衝突判定
        check = BVH.intersect(r, info);
        //各球に判定を行う
        for (int i = 0; i < geometry.size(); ++i)
        {
            IntersectInfo checkinfo;
            //一番近い点を取得する。
            if (geometry[i]->hit(r, checkinfo))
            {
                if (checkinfo.distance < info.distance)
                {
                    info = checkinfo;
                    check = true;
                }
            }
        }

        return check;
    }

    void Scenecheck()
    {
        cout << geometry.size() << endl;
    }

    vec3f worldBackGround(Ray &r)
    {
        if (worldsphere != nullptr)
        {
            return worldsphere->hit(r);
        }
        return vec3f(0.0);
    }
};

#endif