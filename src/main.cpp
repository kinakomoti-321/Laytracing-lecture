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

int main()
{
  Image img(512, 512);
  const unsigned int width = 512;
  const unsigned int height = 512;

  pincamera camera(vec3f(0, 0, 5), vec3f(0, 0, -1));
  Scene scene;
  vec3f light = vec3f(0, 1, 1);
  light = normalize(light);
  scene.SphereAdd(vec3f(-1, 0, 1), 1.0f, vec3f(1, 0, 0), MaterialType::Diffuse);
  scene.SphereAdd(vec3f(1, 0, -1), 1.0f, vec3f(0, 1, 0), MaterialType::Diffuse);
  scene.SphereAdd(vec3f(0), 1.0f);
  scene.LightAdd(light);
  for (int j = 0; j < height; ++j)
  {
    for (int i = 0; i < width; ++i)
    {
      const float u = (2.0f * i - width) / width;
      const float v = (2.0f * j - height) / height;
      Ray r = camera.getray(v, u);
      img.SetPixel(i, j, 0.5f * (r.getdirection() + vec3f(1.0f)));
      IntersectInfo info;
      if (scene.hit(r, info))
      {
        img.SetPixel(i, j, info.color);
      }
      else
      {
        img.SetPixel(i, j, vec3f(0));
      }
    }
  }
  img.writePPM("output.ppm");
  return 0;
}