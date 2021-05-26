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
#include "BSDF.h"
#include "BVH/simpleBVH.h"

using namespace std;
class Scene
{
private:
    vector<Geometry *> geometry;
    SimpleBVH BVH;

public:
    Scene(Polygon &polygon) : BVH(SimpleBVH(polygon)) {}
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

    // void hint()
    // {
    //     cout << BVH.nInternalNodes() << endl;
    //     cout << BVH.nLeafNodes() << endl;
    //     cout << BVH.nNodes() << endl;
    // }
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

    // void PyramidAdd(const vec3f vertex[4], const vec3f color, const MaterialType mater)
    // {
    //     TriangleAdd(vertex[0], vertex[1], vertex[2], color, mater);
    //     TriangleAdd(vertex[0], vertex[1], vertex[3], color, mater);
    //     TriangleAdd(vertex[1], vertex[2], vertex[3], color, mater);
    //     TriangleAdd(vertex[0], vertex[2], vertex[3], color, mater);
    // }
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
        float min = 10000;
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

    void Scenecheck()
    {
        cout << geometry.size() << endl;
    }
};
#endif