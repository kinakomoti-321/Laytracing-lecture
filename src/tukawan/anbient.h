#include "scene.h"
#include "vec3.h"
#include "rng.h"
#include "geometry.h"
#include <iostream>
#include <algorithm>
#ifndef _ANBIENT_
#define _ANBIENT_

float P = 3.14159265359f;
// ローカル座標系からワールド座標系への変換
vec3f localToWorld(const vec3f &v, const vec3f &lx, const vec3f &ly,
                   const vec3f &lz)
{
    return vec3f(v[0] * lx[0] + v[1] * ly[0] + v[2] * lz[0],
                 v[0] * lx[1] + v[1] * ly[1] + v[2] * lz[1],
                 v[0] * lx[2] + v[1] * ly[2] + v[2] * lz[2]);
}

// 法線から接空間の基底を計算する
void tangentSpaceBasis(const vec3f &n, vec3f &t, vec3f &b)
{
    if (n[1] < 0.9f)
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

// 接空間での半球面一様サンプリング
vec3f sampleHemisphere(float u, float v)
{
    const float theta = std::acos(clamp(1.0f - 2.0f * u, -1.0f, 1.0f)) / 2.0f;
    const float phi = 2.0f * P * v;
    return vec3f(std::cos(phi) * std::sin(theta), std::cos(theta),
                 std::sin(phi) * std::sin(theta));
}

//AO
// float RTAO(const IntersectInfo &info, Scene &scene, RNGrandom &rng)
// {
//     // AOサンプル数
//     constexpr int AO_SAMPLES = 100;
//     // 遮蔽判定距離
//     constexpr float DISTANCE = 10.0f;

//     //拡散率
//     constexpr float p = 1.0;

//     // 遮蔽割合の計算
//     float count = 0;

//     // 接空間の基底の計算
//     vec3f t, b;
//     tangentSpaceBasis(info.normal, t, b);

//     // 半球面一様サンプリング
//     const vec3f direction_tangent =
//         sampleHemisphere(rng.getRandom(), rng.getRandom());

//     // 接空間からワールド座標系への変換
//     const vec3f direction =
//         localToWorld(direction_tangent, t, info.normal, b);

//     // 遮蔽判定
//     Ray shadow_ray(info.position, direction);
//     IntersectInfo shadow_info;
//     if (scene.hit(shadow_ray, shadow_info) && shadow_info.distance < DISTANCE)
//     {
//         count += dot(direction, info.normal);
//     }

//     return 2.0f * p * count;
// }

float RTAO(const IntersectInfo &info, Scene &scene, RNGrandom &rng, Ray &nextray)
{
    // AOサンプル数
    constexpr int AO_SAMPLES = 100;
    // 遮蔽判定距離
    constexpr float DISTANCE = 10.0f;

    //拡散率
    constexpr float p = 0.9;

    // 遮蔽割合の計算
    float count = 0;

    // 接空間の基底の計算
    vec3f t, b;
    tangentSpaceBasis(info.normal, t, b);

    // 半球面一様サンプリング
    const vec3f direction_tangent =
        sampleHemisphere(rng.getRandom(), rng.getRandom());

    // 接空間からワールド座標系への変換
    const vec3f direction =
        localToWorld(direction_tangent, t, info.normal, b);
    nextray = Ray(info.position, direction);
    // IntersectInfo next_info;
    // // float LE = 1;
    // // if (scene.hit(next_ray, next_info))
    // // {
    // //     LE = 0;
    // // }

    return 2.0f * p * dot(direction, info.normal);
}

vec3f Pathtracer(Ray &r, Scene &scene, RNGrandom &rng)
{
    //ロシアンルーレット用の確率
    float p = 0.99;
    //最大反射回数
    int MaxRef = 100;

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
            LTE = 0;
            break;
        }

        if (info.geometry->getMaterial() == MaterialType::Emission)
        {
            LTE = (float)3.0 * s;
            break;
        }

        // 接空間の基底の計算
        vec3f t, b;
        tangentSpaceBasis(info.normal, t, b);

        // 半球面一様サンプリング
        const vec3f direction_tangent =
            sampleHemisphere(rng.getRandom(), rng.getRandom());

        // 接空間からワールド座標系への変換
        const vec3f nextdirection =
            localToWorld(direction_tangent, t, info.normal, b);

        ray = Ray(info.position + nextdirection * 0.01f, nextdirection);
        //衝突した場所の輝度計算
        float cos = abs(dot(info.normal, nextdirection));
        float pdf = cos / P;
        s = adamarl(s, info.geometry->BSDF() * cos / pdf);

        counter++;
    }

    return LTE;
}

// using Vec3f = vec3f;
// //ref sample
// // Primary Rayを入力とし, 放射輝度をパストレーシングで計算して返す
// Vec3f pathTracing(const Ray &ray_in, Scene &scene, RNGrandom &rng)
// {
//     constexpr int maxDepth = 100;                // 最大反射回数
//     constexpr float russianRouletteProb = 0.99f; // ロシアンルーレットの確率
//     Vec3f rho(0.9f);                             // Lambert BRDFのrho

//     // path tracing
//     Vec3f radiance = 0;           // 放射輝度
//     Vec3f throughput = {1, 1, 1}; // f*cos / pdfの積
//     Ray ray = ray_in;

//     //test
//     float counter = 0;
//     for (int i = 0; i < maxDepth; ++i)
//     {
//         // ロシアンルーレット
//         if (rng.getRandom() > russianRouletteProb)
//         {
//             break;
//         }
//         throughput = throughput / russianRouletteProb;

//         // レイを飛ばして交差点を計算
//         IntersectInfo info;
//         if (!scene.hit(ray, info))
//         {
//             // 空に飛んでいった場合
//             radiance = radiance + throughput;
//             break;
//         }

//         // 接空間の基底の計算
//         Vec3f t, b;
//         tangentSpaceBasis(info.normal, t, b);
//         // 半球面一様サンプリング
//         const Vec3f directionTangent =
//             sampleHemisphere(rng.getRandom(), rng.getRandom());
//         // 接空間からワールド座標系への変換
//         const Vec3f direction =
//             localToWorld(directionTangent, t, info.normal, b);

//         // BRDFの計算
//         const Vec3f brdf = rho / P;
//         // cosの計算
//         const float cos = std::abs(dot(direction, info.normal));
//         // p.d.f.の計算
//         const float pdf = 1.0f * cos / P;

//         // throughputの更新
//         throughput = adamarl(throughput, brdf * cos / pdf);

//         // 次のレイの生成
//         ray = Ray(info.position, direction);
//         counter++;
//     }
//     return radiance;
// }
#endif
