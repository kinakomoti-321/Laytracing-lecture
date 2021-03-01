#include <iostream>
#include <algorithm>
#include <cmath>
#include "scene.h"
#include "vec3.h"
#include "image.h"
#include "ray.h"
#include "pinhole-camera.h"
#include "sphere.h"
#include "intersect-info.h"
#include "color.h"
#include "rng.h"
#include <omp.h>

using namespace std;
using vec3f = vec3<float>;
constexpr float PI = 3.14159265359f;
vec3f raytracing(Ray &ray, Scene &scene, RNGrandom &rng);

vec3f localToWorld(const vec3f &v, const vec3f &lx, const vec3f &ly,
                   const vec3f &lz)
{
  return vec3f(v[0] * lx[0] + v[1] * ly[0] + v[2] * lz[0],
               v[0] * lx[1] + v[1] * ly[1] + v[2] * lz[1],
               v[0] * lx[2] + v[1] * ly[2] + v[2] * lz[2]);
}
float AOratio(Scene &scene, const vec3f &normal, vec3f &position, RNGrandom &rng)
{
  constexpr int sample = 100;
  constexpr float distance = 10.0f;

  int count = 0;
  vec3f t, b;
  if (normal[1] < 0.9f)
  {
    t = closs(normal, vec3f(0, 1, 0));
  }
  else
  {
    t = closs(normal, vec3f(0, 0, -1));
  }
  t = normalize(t);
  b = closs(t, normal);
  b = normalize(b);
  for (int i = 0; i < sample; ++i)
  {

    float u = rng.getRandom();
    float v = rng.getRandom();
    float theta = acos(1.0f - u);
    float phai = 2.0f * PI * v;

    float x = cos(phai) * sin(theta);
    float y = 1.0f - u;
    float z = sin(phai) * cos(theta);

    vec3f local = vec3f(x, y, z);
    vec3f world = localToWorld(local, t, normal, b);
    world = normalize(world);
    Ray shadow_ray(position, world);
    IntersectInfo shadow_info;
    vec3f checknormal(0, 0, 1);
    checknormal = localToWorld(checknormal, t, normal, b);

    if (checknormal == normal)
      cout << "check" << endl;
    if (scene.hit(shadow_ray, shadow_info) && shadow_info.distance < distance)
    {
      ++count;
    }
  }
  return static_cast<float>(count) / sample;
}

int main()
{
  constexpr int sampling = 4;
  const unsigned int width = 512;
  const unsigned int height = 512;
  Image img(width, height);
  vec3f camPos(4, 1, 7);
  vec3f lookAt(0);
  lookAt = lookAt - camPos;
  lookAt = normalize(lookAt);
  pincamera camera(camPos, lookAt);

  Scene scene;
  scene.SphereAdd(vec3f(0, -1001, 0), 1000.0, vec3f(0.9), MaterialType::Diffuse);
  scene.SphereAdd(vec3f(-2, 0, 1), 1.0f, vec3f(0.8, 0.2, 0.2), MaterialType::Diffuse);
  scene.SphereAdd(vec3f(0), 1.0, vec3f(0.2, 0.8, 0.2), MaterialType::Diffuse);
  scene.SphereAdd(vec3f(2, 0, -1), 1.0, vec3f(0.2, 0.2, 0.8), MaterialType::Diffuse);
  scene.SphereAdd(vec3f(-2, 3, 1), 1.0, vec3f(1), MaterialType::Mirror);
  scene.SphereAdd(vec3f(3, 1, 2), 1.0, vec3f(1), MaterialType::Glass);

#pragma omp parallel for schedule(dynamic, 1)
  for (int j = 0; j < height; ++j)
  {
    for (int i = 0; i < width; ++i)
    {
      RNGrandom rng(i + width * j);
      vec3f samplecolor(0);
      for (int k = 0; k < sampling; ++k)
      {
        const float u = (2.0f * (i + rng.getRandom() - 0.5f) - width) / width;
        const float v = (2.0f * (j + rng.getRandom() - 0.5f) - height) / height;
        Ray r = camera.getray(v, u);
        samplecolor = samplecolor + raytracing(r, scene, rng);
      }
      samplecolor = samplecolor / static_cast<float>(sampling);
      img.SetPixel(i, j, samplecolor);
    }
  }

  img.writePPM("output.ppm");
  return 0;
}

vec3f raytracing(Ray &r, Scene &scene, RNGrandom &rng)
{
  int Max_Depth = 100;
  vec3f light(1, 1, 1);
  light = normalize(light);
  IntersectInfo info;
  Ray ray = r;
  float refractance[2] = {1.0f, 1.5f};
  for (int i = 1; i <= Max_Depth; ++i)
  {
    if (scene.hit(ray, info))
    {
      return (1.0f - AOratio(scene, info.normal, info.position, rng)) * vec3f(1, 1, 1);
      // MaterialType type = info.sphere->getMaterial();
      // if (type == MaterialType::Diffuse)
      // {
      //   Ray lightray(info.position, light);
      //   vec3f anb = info.sphere->getColor();

      //   if (scene.hit(lightray, info))
      //   {
      //     return 0.2f * anb;
      //   }
      //   else
      //   {
      //     return max(dot(light, info.normal), 0.0f) * info.sphere->getColor() + 0.2f * anb;
      //   }
      // }
      // else if (type == MaterialType::Glass)
      // {
      //   bool is_inside = dot(-ray.getdirection(), info.normal) < 0;
      //   vec3f next_direction;
      //   if (!is_inside)
      //   {
      //     if (!refraction(-ray.getdirection(), 1.0f, info.normal, 1.5f, next_direction))
      //     {
      //       next_direction = refrect(ray.getdirection(), info.normal);
      //     }
      //   }
      //   else
      //   {
      //     if (!refraction(-ray.getdirection(), 1.5f, -info.normal, 1.0f, next_direction))
      //     {
      //       next_direction = refrect(ray.getdirection(), -info.normal);
      //     }
      //   }

      //   ray = Ray(info.position, next_direction);
      // }
      // else if (type == MaterialType::Mirror)
      // {
      //   vec3f rdirection = refrect(ray.getdirection(), info.normal);
      //   ray = Ray(info.position, rdirection);
      // }
    }
    else
    {
      return vec3f(0, 0, 0);
    }
  }

  return vec3f(0, 0, 0);
}
