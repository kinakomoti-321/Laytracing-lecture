#include "scene.h"
#include "vec3.h"
#include "rng.h"
#include <iostream>

constexpr float PI = 3.14159265359f;
// vec3f localToWorld(const vec3f &v, const vec3f &lx, const vec3f &ly,

//                    const vec3f &lz)
// {
//     return vec3f(v[0] * lx[0] + v[1] * ly[0] + v[2] * lz[0],
//                  v[0] * lx[1] + v[1] * ly[1] + v[2] * lz[1],
//                  v[0] * lx[2] + v[1] * ly[2] + v[2] * lz[2]);
// }

// float AOratio(Scene &scene, const vec3f &normal, vec3f &position, RNGrandom &rng)
// {
//     constexpr int sample = 100;
//     constexpr float distance = 10.0f;

//     int count = 0;
//     vec3f t, b;
//     if (normal[1] < 0.9f)
//     {
//         t = closs(normal, vec3f(0, 1, 0));
//     }
//     else
//     {
//         t = closs(normal, vec3f(0, 0, -1));
//     }
//     t = normalize(t);
//     b = closs(t, normal);
//     b = normalize(b);
//     for (int i = 0; i < sample; ++i)
//     {

//         float u = rng.getRandom();
//         float v = rng.getRandom();
//         float theta = acos(1.0f - u);
//         float phai = 2.0f * PI * v;

//         float x = cos(phai) * sin(theta);
//         float y = 1.0f - u;
//         float z = sin(phai) * cos(theta);

//         vec3f local = vec3f(x, y, z);
//         vec3f world = localToWorld(local, t, normal, b);
//         world = normalize(world);
//         Ray shadow_ray(position, world);
//         IntersectInfo shadow_info;
//         vec3f checknormal(0, 0, 1);
//         checknormal = localToWorld(checknormal, t, normal, b);

//         if (checknormal == normal)
//             cout << "check" << endl;
//         if (scene.hit(shadow_ray, shadow_info) && shadow_info.distance < distance)
//         {
//             ++count;
//         }
//     }
//     return static_cast<float>(count) / sample;
// }

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
    const float theta = std::acos(std::max(1.0f - u, 0.0f));
    const float phi = 2.0f * PI * v;
    return vec3f(std::cos(phi) * std::sin(theta), 1.0f - u,
                 std::sin(phi) * std::sin(theta));
}

// AO
float RTAO(const IntersectInfo &info, Scene &scene, RNGrandom &rng)
{
    // AOサンプル数
    constexpr int AO_SAMPLES = 100;
    // 遮蔽判定距離
    constexpr float DISTANCE = 10.0f;

    // 遮蔽割合の計算
    int count = 0;
    for (int i = 0; i < AO_SAMPLES; ++i)
    {
        // 接空間の基底の計算
        vec3f t, b;
        tangentSpaceBasis(info.normal, t, b);

        // 半球面一様サンプリング
        const vec3f direction_tangent =
            sampleHemisphere(rng.getRandom(), rng.getRandom());

        // 接空間からワールド座標系への変換
        const vec3f direction =
            localToWorld(direction_tangent, t, info.normal, b);

        // 遮蔽判定
        Ray shadow_ray(info.position, direction);
        IntersectInfo shadow_info;
        if (scene.hit(shadow_ray, shadow_info) && shadow_info.distance < DISTANCE)
        {
            count++;
        }
    }

    return static_cast<float>(count) / AO_SAMPLES;
}
