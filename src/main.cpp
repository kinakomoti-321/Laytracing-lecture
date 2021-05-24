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
#include "BVH/polygon.h"
#include "BSDF.h"
#include <omp.h>
#include <math.h>
using namespace std;
using vec3f = vec3<float>;

const float PI = 3.14159234;

string objname = "teapot.obj";

int main()
{

  int sampling = 1000;
  const unsigned int width = 512;
  const unsigned int height = 512;
  Image img(width, height);
  vec3f camPos(0, 0, 7.5);
  vec3f lookAt(0, 0, 0);
  lookAt = lookAt - camPos;
  lookAt = normalize(lookAt);
  pincamera camera(camPos, lookAt);

  vector<vec3f> vertex1 = {
      vec3f(1, 1, 1),
      vec3f(1, -1, 1),
      vec3f(-1, -1, 1), vec3f(-1, 1, 1), vec3f(1, 1, -1), vec3f(1, -1, -1), vec3f(-1, -1, -1), vec3f(-1, 1, -1)};
  float vertex2[] = {
      1,
      1,
      1,
      1,
      -1,
      1,
      -1,
      -1,
      1,
      -1,
      1,
      1,
      1,
      1,
      -1,
      1,
      -1,
      -1,
      -1,
      -1,
      -1,
      -1,
      1,
      -1,
  };
  unsigned int index[] = {0, 1, 2, 0, 2, 3, 0, 7, 4, 0, 3, 7, 3, 2, 6, 3, 6, 7, 1, 5, 6, 1, 6, 2, 0, 5, 1, 0, 4, 5, 4, 6, 7, 4, 5, 6};
  vector<int> index1 = {
      0,
      7,
      4,
      0,
      3,
      7,
      3,
      2,
      6,
      3,
      6,
      7,
      1,
      5,
      6,
      1,
      6,
      2,
      0,
      5,
      1,
      0,
      4,
      5,
  };
  vector<int> index2 = {0, 1, 2, 0, 2, 3, 4, 6, 7, 4, 5, 6};
  for (int i = 0; i < vertex1.size(); i++)
  {
    vertex1[i] = MoveMat(vec3f(0, 8.0, 0)) * ScaleMat(vec3f(2.0)) * vertex1[i];
  }
  vector<BSDF *> bsdf;
  bsdf.push_back(new Lambert(vec3f(0.9)));
  bsdf.push_back(new Lambert(vec3f(0.9, 0.2, 0.2)));
  bsdf.push_back(new Lambert(vec3f(0.2, 0.9, 0.2)));
  bsdf.push_back(new Lambert(vec3f(0.2, 0.2, 0.9)));
  bsdf.push_back(new Mirror(vec3f(0.9f)));
  bsdf.push_back(new Glass(vec3f(1.0), 1.54));
  vec3f col(0.9);
  Polygon poly(36, vertex2, index, col);
  Scene scene;
  vector<int> index3(std::begin(index), std::end(index));
  vector<vec3f> vertexlight = {vec3f(1, 0, 1), vec3f(1, 0, -1), vec3f(-1, 0, 1), vec3f(-1, 0, -1)};
  vector<int> indexlight = {0, 1, 2, 1, 2, 3};

  for (int i = 0; i < vertexlight.size(); i++)
  {
    vertexlight[i] = MoveMat(vec3f(0, 7.8, 0)) * ScaleMat(vec3f(4)) * vertexlight[i];
  }
  scene.polygon(vertexlight, indexlight, vec3f(0.9), MaterialType::Emission, bsdf[0]);

  for (int i = 0; i < vertex1.size(); i++)
  {
    vertex1[i] = ScaleMat(vec3f(4.0)) * MoveMat(vec3f(0, -8, 0)) * vertex1[i];
  }
  vector<int> index4 = {1, 2, 5, 2, 5, 6};
  scene.polygon(vertex1, index1, vec3f(0.9), MaterialType::Diffuse, bsdf[0]);
  scene.polygon(vertex1, index2, vec3f(0), MaterialType::Mirror, bsdf[4]);
  scene.SphereAdd(MoveMat(vec3f(0, -4, -6)) * vec3f(-5, 0, 0), 2.0f, vec3f(0.8, 0.2, 0.2), MaterialType::Diffuse, bsdf[4]);
  scene.SphereAdd(MoveMat(vec3f(0, -4, -6)) * vec3f(5, 0, 0), 2.0, vec3f(0.2, 0.2, 0.8), MaterialType::Diffuse, bsdf[2]);
  scene.SphereAdd(MoveMat(vec3f(0, -4, -6)) * vec3f(0), 2.0f, vec3f(0.2, 0.8, 0.2), MaterialType::Diffuse, bsdf[3]);
  scene.SphereAdd(vec3f(2, -2, -2), 2.0, vec3f(1), MaterialType::Glass, bsdf[5]);
  // scene.RectangleAdd(MoveMat(vec3f(0, 8, 0)) * vec3f(0), vec3f(0, -1, 0), vec3f(0.9), MaterialType::Diffuse);
  cout << "Scene Set Complete" << endl;
  cout << "Pathtracing start" << endl;
  int i = 0;
// #pragma omp parallel for schedule(dynamic, 1)
#pragma omp parallel for private(i) collapse(2)
  for (int j = 0; j < height; ++j)
  {
    for (i = 0; i < width; ++i)
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
        // cout << samplecolor << endl;
        // samplecolor = samplecolor - normalCheck(r, scene);
      }
      samplecolor = samplecolor / static_cast<float>(sampling);
      img.SetPixel(i, j, samplecolor);
    }
  }
  cout << "end" << endl;
  scene.Scenecheck();
  for (int i = 0; i < bsdf.size(); i++)
  {
    delete bsdf[i];
  }
  img.writePPM("output.ppm");
  return 0;
}
