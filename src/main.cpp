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
#include "rng.h"
#include "geometry.h"
#include "matrix.h"
#include "pathtracer.h"
#include <omp.h>
#include <math.h>
using namespace std;
using vec3f = vec3<float>;

const float PI = 3.14159234;

string objname = "teapot.obj";

int main()
{

  int sampling = 100;
  const unsigned int width = 512;
  const unsigned int height = 512;
  Image img(width, height);
  vec3f camPos(0, -1, 7);
  vec3f lookAt(0, -1, 0);
  lookAt = lookAt - camPos;
  lookAt = normalize(lookAt);
  pincamera camera(camPos, lookAt);
  Scene scene;

  vector<vec3f> vertex1 = {
      vec3f(1, 1, 1),
      vec3f(1, -1, 1),
      vec3f(-1, -1, 1), vec3f(-1, 1, 1), vec3f(1, 1, -1), vec3f(1, -1, -1), vec3f(-1, -1, -1), vec3f(-1, 1, -1)};
  vector<vec3f> vertex2 = vertex1;
  vector<int> index = {0, 1, 2, 0, 2, 3, 0, 7, 4, 0, 3, 7, 3, 2, 6, 3, 6, 7, 1, 5, 6, 1, 6, 2, 0, 5, 1, 0, 4, 5, 4, 6, 7, 4, 5, 6};
  vector<int> index1 = {0, 1, 2, 0, 2, 3};
  for (int i = 0; i < 8; i++)
  {
    vec3f d = vertex2[i];
    cout << vertex2[i] << endl;
    vertex2[i] = ScaleMat(vec3f(8.0)) * d;
    cout << vertex2[i] << endl;
  }

  // scene.Polygon(vertex2, index1, vec3f(0.9f), MaterialType::Diffuse);
  // index1 = {1, 2, 5, 2, 6, 5};
  // scene.Polygon(vertex2, index1, vec3f(0.9f), MaterialType::Diffuse);
  // index1 = {2, 6, 3, 6, 7, 3};
  // scene.Polygon(vertex2, index1, vec3f(0.9f), MaterialType::Diffuse);
  // index1 = {3, 0, 4, 3, 4, 7};
  // scene.Polygon(vertex2, index1, vec3f(0.9f), MaterialType::Diffuse);
  // index1 = {0, 4, 1, 1, 5, 4};
  // scene.Polygon(vertex2, index1, vec3f(0.9f), MaterialType::Diffuse);
  // index1 = {4, 5, 6, 7, 4, 6};
  // scene.Polygon(vertex2, index1, vec3f(0.9f), MaterialType::Diffuse);
  for (int i = 0; i < 8; i++)
  {
    vec3f d = vertex1[i];
    vertex1[i] = MoveMat(vec3f(0, 4, 0)) * d;
  }

  // scene.Polygon(vertex2, index1, vec3f(0.9), MaterialType::Diffuse);
  scene.SphereAdd(MoveMat(vec3f(0, -4, 0)) * vec3f(-2, 0, 1), 2.0f, vec3f(0.8, 0.2, 0.2), MaterialType::Diffuse);
  // scene.Polygon(vertex1, index, vec3f(1), MaterialType::Emission);
  scene.SphereAdd(MoveMat(vec3f(0, -4, 0)) * vec3f(2, 0, -1), 2.0, vec3f(0.2, 0.2, 0.8), MaterialType::Diffuse);

  int i = 0;
// #pragma omp parallel for schedule(dynamic, 1)
#pragma omp parallel for private(i) collapse(2)
  for (int j = 0; j < height; ++j)
  {
    for (
        i = 0;
        i < width; ++i)
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
        //samplecolor = normalCheck(r, scene);
      }
      samplecolor = samplecolor / static_cast<float>(sampling);
      img.SetPixel(i, j, samplecolor);
    }
  }

  img.writePPM("output.ppm");
  return 0;
}
