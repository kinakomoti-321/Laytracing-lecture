
#define TINYOBJLOADER_IMPLEMENTATION
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
#include "BSDF.h"
#include "BVH/AABB.h"
#include "../extern/tinyobjloader/tiny_obj_loader.h"
#include "Picture/Texture.h"
#include <omp.h>
#include <math.h>
#include <chrono>
using namespace std;
using vec3f = vec3<float>;

const float PI = 3.14159234;

const string cornel1 = "sphere1.obj";
const string cornel2 = "sphere2.obj";
const string cornel3 = "plane.obj";
const string light = "light.obj";
const string worldground = "ground.hdr";
const string red = "red.jpg";
const string white = "white.jpg";
const string green = "green.jpg";
bool loadObj(const std::string &filename, std::vector<float> &vertices,
             std::vector<unsigned int> &indices, std::vector<float> &normals,
             std::vector<float> &uvs)
{
  tinyobj::ObjReader reader;
  cout << "test" << endl;
  if (!reader.ParseFromFile(filename))
  {
    if (!reader.Error().empty())
    {
      std::cerr << reader.Error();
    }
    return false;
  }

  if (!reader.Warning().empty())
  {
    std::cout << reader.Warning();
  }

  const auto &attrib = reader.GetAttrib();
  const auto &shapes = reader.GetShapes();

  vertices = attrib.vertices;
  if (attrib.normals.size() == attrib.vertices.size())
  {
    normals = attrib.normals;
  }
  // if (attrib.texcoords.size() == (attrib.vertices.size() / 3) * 2)
  // {
  uvs = attrib.texcoords;
  // for (int i = 0; i < attrib.texcoords.size(); i++)
  // {
  //   cout << attrib.texcoords[i] << endl;
  // }
  // cout << endl;
  //}

  for (size_t s = 0; s < shapes.size(); ++s)
  {
    for (const auto &idx : shapes[s].mesh.indices)
    {
      indices.push_back(idx.vertex_index);
    }
  }

  return true;
}

int main()
{
  int sampling = 100;
  const unsigned int width = 512;
  const unsigned int height = 512;
  vector<float> cornelvert[3], lightvert;
  vector<unsigned int> cornelind[3], lightind;
  vector<float> cornelnormal[3], lightnormal;
  vector<float> corneluv[3], lightuv;

  if (!loadObj(cornel1, cornelvert[0], cornelind[0], cornelnormal[0], corneluv[0]))
    return 0;

  if (!loadObj(cornel2, cornelvert[1], cornelind[1], cornelnormal[1], corneluv[1]))
    return 0;
  if (!loadObj(cornel3, cornelvert[2], cornelind[2], cornelnormal[2], corneluv[2]))
    return 0;
  if (!loadObj(light, lightvert, lightind, lightnormal, lightuv))
    return 0;
  Image img(width, height);
  vec3f camPos(40, 15, 0);
  vec3f lookAt(-1, 15, 0);
  lookAt = lookAt - camPos;
  lookAt = normalize(lookAt);
  pincamera camera(camPos, lookAt);

  vector<BSDF *> bsdf;
  bsdf.push_back(new Lambert(vec3f(0.9)));
  bsdf.push_back(new Lambert(vec3f(0.9, 0.2, 0.2)));
  bsdf.push_back(new Lambert(vec3f(0.2, 0.9, 0.2)));
  bsdf.push_back(new Lambert(vec3f(0.9, 0.2, 0.2)));
  bsdf.push_back(new Mirror(vec3f(0.9f)));
  bsdf.push_back(new Glass(vec3f(1.0), 1.54));
  bsdf.push_back(new TorranceSparrow(vec3f(0.94), vec3f(1.00, 0.89, 0.57), 1.0));
  bsdf.push_back(new TorranceSparrow(vec3f(0.94), vec3f(1.0, 0.89, 0.57), 1.0));
  bsdf.push_back(new TorranceSparrow(vec3f(0.94), vec3f(0.94), 0.5));
  vec3f col(0.9);
  Texture tex1(red);
  Texture tex2(green);
  Texture tex3(white);
  Polygon poly1(cornelind[0].size(), cornelvert[0].data(), cornelind[0].data(), MaterialType::Diffuse, bsdf[8]);
  Polygon poly2(cornelind[1].size(), cornelvert[1].data(), cornelind[1].data(), MaterialType::Diffuse, bsdf[7]);
  Polygon poly3(cornelind[2].size(), cornelvert[2].data(), cornelind[2].data(), MaterialType::Diffuse, bsdf[0]);
  Polygon poly4(lightind.size(), lightvert.data(), lightind.data(), MaterialType::Emission, bsdf[0]);
  Scene scene(poly1);
  scene.addPolygonBVH(poly2);
  scene.addPolygon(&poly3);
  scene.addPolygon(&poly4);
  WorldTexture worldtex(worldground);
  WorldSphere worlds(&worldtex);
  scene.worldSet(&worlds);
  cout << "Scene Set Complete" << endl;
  cout << "Pathtracing start" << endl;
  scene.Build();

  int i = 0;
  // #pragma omp parallel for schedule(dynamic, 1)

  auto start = std::chrono::system_clock::now();
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

        samplecolor = samplecolor + Pathtracer(r, scene, rng);
        // samplecolor = samplecolor + texcheck(r, scene);
        // outcheck(r, scene);
        // samplecolor = samplecolor + uvcheck(r, scene);
        // samplecolor = samplecolor + normalCheck(r, scene);
        // float u1 = i / (float)width;
        // float v1 = j / (float)height;
        // // cout << v1 << " " << u1 << endl;
        // samplecolor = samplecolor + tex1.getPixel(u1, v1);
        // cout << i << j << endl;
      }
      samplecolor = samplecolor / static_cast<float>(sampling);
      img.SetPixel(i, j, samplecolor);
    }
  }
  cout << "end" << endl;
  auto end = std::chrono::system_clock::now();
  auto msec = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  cout << "sampling : " << sampling << endl
       << "rendering time : " << msec << "ms" << endl;
  scene.Scenecheck();
  for (int i = 0; i < bsdf.size(); i++)
  {
    delete bsdf[i];
  }
  img.writePPM("output.ppm");
  return 0;
}