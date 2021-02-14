#include <iostream>
#include <algorithm>
#include "scene.h"
#include "vec3.h"
#include "image.h"
#include "ray.h"
#include "pinhole-camera.h"
#include "sphere.h"
#include "intersect-info.h"
#include "color.h"

using namespace std;
using vec3f = vec3<float>;
vec3f raytracing(Ray &ray, Scene &scene);
int main()
{
  Image img(512, 512);
  const unsigned int width = 512;
  const unsigned int height = 512;

  vec3f camPos(4, 1, 7);
  vec3f lookAt(0);
  lookAt = lookAt - camPos;
  lookAt = normalize(lookAt);
  pincamera camera(camPos, lookAt);

  Scene scene;
  scene.SphereAdd(vec3f(0, -1005, 0), 1000.0, vec3f(0.9), MaterialType::Diffuse);
  scene.SphereAdd(vec3f(-2, 0, 1), 1.0f, vec3f(0.8, 0.2, 0.2), MaterialType::Diffuse);
  scene.SphereAdd(vec3f(0, 1, 0), 1.0, vec3f(0.2, 0.8, 0.2), MaterialType::Diffuse);
  scene.SphereAdd(vec3f(-2, 3, 1), 1.0, vec3f(1), MaterialType::Mirror);
  scene.SphereAdd(vec3f(3, 1, 2), 1.0, vec3f(1), MaterialType::Glass);
  for (int j = 0; j < height; ++j)
  {
    for (int i = 0; i < width; ++i)
    {
      const float u = (2.0f * i - width) / width;
      const float v = (2.0f * j - height) / height;
      Ray r = camera.getray(v, u);
      img.SetPixel(i, j, raytracing(r, scene));
    }
  }
  vec3f v(1, 2, 1);
  v = normalize(v);
  vec3f n(0, 0, 1);
  cout << v << refraction(v, 0.9f, n, 1.0f) << endl;
  img.writePPM("output.ppm");
  return 0;
}

vec3f raytracing(Ray &ray, Scene &scene)
{
  vec3f light(1, 1, 1);
  light = normalize(light);
  IntersectInfo info;
  if (scene.hit(ray, info))
  {
    MaterialType type = info.sphere->getMaterial();
    if (type == MaterialType::Diffuse)
    {
      Ray lightray(info.position, light);
      if (scene.hit(lightray, info))
      {
        return vec3f(0.2, 0.2, 0.2);
      }
      else
      {

        return max(dot(light, info.normal), 0.0f) * info.sphere->getColor();
      }
    }
    else if (type == MaterialType::Glass)
    {
      vec3f direction = ray.getdirection();
      // vec3f rdirection = refraction(direction, 1.0f, info.normal, 0.9f);
      vec3f rdirection = refrect(direction, info.normal);
      Ray ref(info.position, rdirection);
      return raytracing(ref, scene);
    }
    else if (type == MaterialType::Mirror)
    {

      vec3f direction = ray.getdirection();
      vec3f rdirection = refrect(direction, info.normal);
      Ray ref(info.position, rdirection);
      return raytracing(ref, scene);
    }
  }
  else
    return vec3f(0, 0, 0);
}