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
#include "anbient.h"
#include "geometry.h"
#include "raytracer.h"
#include <omp.h>
#include <math.h>
using namespace std;
using vec3f = vec3<float>;

const float PI = 3.14159234;

string objname = "teapot.obj";

int main()
{
  constexpr int sampling = 1000;
  const unsigned int width = 512;
  const unsigned int height = 512;
  Image img(width, height);
  vec3f camPos(4, 1, 7);
  vec3f lookAt(0);
  lookAt = lookAt - camPos;
  lookAt = normalize(lookAt);
  pincamera camera(camPos, lookAt);
  Scene scene;
  // vec3f vertex[4];
  // for (int i = 0; i < 3; ++i)
  // {
  //   vertex[i] = vec3f(2 * cos(2 * PI * i / 3 + 2 * PI / 6), 0, 2 * sin(2 * PI * i / 3 + 2 * PI / 6));
  // }

  // vertex[3] = vec3f(0, 2, 0);
  // scene.PyramidAdd(vertex, vec3f(1, 0, 0), MaterialType::Diffuse);
  vector<vec3f> vertex1 = {
      vec3f(1, 1, 1),
      vec3f(1, -1, 1),
      vec3f(-1, -1, 1), vec3f(-1, 1, 1), vec3f(1, 1, -1), vec3f(1, -1, -1), vec3f(-1, -1, -1), vec3f(-1, 1, 0)};
  vector<int> index = {0, 1, 2, 0, 2, 3, 0, 7, 4, 0, 3, 7, 3, 2, 6, 3, 6, 7, 1, 5, 6, 1, 6, 2, 0, 5, 1, 0, 4, 5, 4, 6, 7, 4, 5, 6};

  scene.Polygon(vertex1, index, vec3f(1), MaterialType::Diffuse);
  for (int i = 0; i < 8; i++)
  {
    vertex1[i][1] += 3;
    vertex1[i][2] *= 2;
    vertex1[i][0] *= 2;
  }

  scene.SphereAdd(vec3f(-2, 0, 1), 1.0f, vec3f(0.8, 0.2, 0.2), MaterialType::Diffuse);
  scene.Polygon(vertex1, index, vec3f(1), MaterialType::Emission);
  // scene.SphereAdd(vec3f(0, 10.0, 0), 5.0, vec3f(0.6, 0.8, 0.2), MaterialType::Emission);
  scene.SphereAdd(vec3f(2, 0, -1), 1.0, vec3f(0.2, 0.2, 0.8), MaterialType::Diffuse);
  scene.SphereAdd(vec3f(-2, 3, 1), 1.0, vec3f(1), MaterialType::Mirror);
  // scene.SphereAdd(vec3f(3, 1, 2), 1.0, vec3f(1), MaterialType::Diffuse);
  scene.RectangleAdd(vec3f(0, -2, 0), vec3f(0, 1, 0), vec3f(1, 1, 1), MaterialType::Diffuse);

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

        // samplecolor = samplecolor + vec3f(Pathtracer(r, scene, rng));
        samplecolor = samplecolor + Pathtracer(r, scene, rng);
      }
      samplecolor = samplecolor / static_cast<float>(sampling);
      img.SetPixel(i, j, samplecolor);
    }
  }

  img.writePPM("output.ppm");
  return 0;
}
