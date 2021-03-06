#ifndef _PathTracer_
#define _PathTracer_
#include "scene.h"
#include "vec3.h"
#include "rng.h"
#include "geometry.h"
#include <iostream>
#include <algorithm>

float P = 3.14159265359f;
// ローカル座標系からワールド座標系への変換
vec3f localToWorld(const vec3f &v, const vec3f &lx, const vec3f &ly,
                   const vec3f &lz)
{
    return vec3f(v[0] * lx[0] + v[1] * ly[0] + v[2] * lz[0],
                 v[0] * lx[1] + v[1] * ly[1] + v[2] * lz[1],
                 v[0] * lx[2] + v[1] * ly[2] + v[2] * lz[2]);
}

vec3f worldtoLocal(const vec3f &v, const vec3f &lx, const vec3f &ly, const vec3f &lz)
{
    return vec3f(v[0] * lx[0] + v[1] * lx[1] + v[2] * lx[2],
                 v[0] * ly[0] + v[1] * ly[1] + v[2] * ly[2],
                 v[0] * lz[0] + v[1] * lz[1] + v[2] * lz[2]);
}

// 法線から接空間の基底を計算する
void tangentSpaceBasis(const vec3f &n, vec3f &t, vec3f &b)
{
    if (abs(n[1]) < 0.9f)
    {
        t = closs(n, vec3f(0, 1, 0));
    }
    else
    {
        t = closs(n, vec3f(0, 0, -1));
    }
    t = normalize(t);
    b = closs(t, n);
    b = normalize(b);
}

// // 接空間での半球面一様サンプリング
// vec3f sampleHemisphere(float u, float v)
// {
//     const float theta = std::acos(clamp(1.0f - 2.0f * u, -1.0f, 1.0f)) / 2.0f;
//     const float phi = 2.0f * P * v;
//     return vec3f(std::cos(phi) * std::sin(theta), std::cos(theta),
//                  std::sin(phi) * std::sin(theta));
// }

vec3f Pathtracer(Ray &r, Scene &scene, RNGrandom &rng)
{
    //ロシアンルーレット用の確率
    float p = 0.99;
    //最大反射回数
    int MaxRef = 10;

    //計算結果
    vec3f LTE(0.0);
    vec3f s(1.0);
    //レイ
    Ray ray = r;

    float counter = 0;

    for (int i = 0; i < 100; i++)
    {
        //ロシアンルーレット
        if (rng.getRandom() > p)
        {
            break;
        }
        //ロシアンルーレットの次の項の調整
        s /= p;

        //衝突判定　衝突しなかったら脱出
        IntersectInfo info;
        if (!scene.hit(ray, info))
        {
            LTE = adamarl(s, scene.worldBackGround(ray));
            break;
        }

        if (info.geometry->getMaterial() == MaterialType::Emission)
        {

            LTE = (float)5.0 * s;
            break;
        }

        // 接空間の基底の計算
        vec3f t, b;
        tangentSpaceBasis(info.normal, t, b);

        vec3f bsdf;
        vec3f wo = worldtoLocal(-ray.getdirection(), t, info.normal, b);
        float pdf;
        vec3f direction_tangent;
        bsdf = info.poly->getBSDF(rng, wo, direction_tangent, pdf, info.uv[0], info.uv[1]);
        //sampleHemisphere(rng.getRandom(), rng.getRandom());
        // 接空間からワールド座標系への変換
        const vec3f nextdirection =
            localToWorld(direction_tangent, t, info.normal, b);

        ray = Ray(info.position + nextdirection * 0.01f, nextdirection);
        //衝突した場所の輝度計算
        float cos = abs(dot(info.normal, nextdirection));
        //float pdf = cos / P;
        // s = adamarl(s, info.geometry->BSDF() * cos / pdf);
        s = adamarl(s, bsdf * cos / pdf);
        // cout << bsdf << " " << pdf << " " << cos << endl;

        // cout << s << endl;
        counter++;
    }
    // cout << "pathtracer end" << endl;
    return LTE;
}

vec3f normalCheck(Ray &r, Scene &scene)
{
    IntersectInfo info;
    if (scene.hit(r, info))
    {
        // return adamarl(info.normal, info.normal);
        return info.normal;
    }

    return vec3f(1.0);
}

vec3f uvcheck(Ray &r, Scene &scene)
{
    IntersectInfo info;
    if (scene.hit(r, info))
    {
        return vec3f(info.uv[0], info.uv[1], 0.0);
    }
    return vec3f(1.0);
}

vec3f texcheck(Ray &r, Scene &scene)
{
    IntersectInfo info;
    if (scene.hit(r, info))
    {
        return info.poly->tex->getTex(info.uv[0], info.uv[1]);
    }

    return vec3f(0.0);
}
#endif