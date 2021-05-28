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
#include <omp.h>
#include <math.h>
using namespace std;
using vec3f = vec3<float>;

const float PI = 3.14159234;

string objname = "teapot.obj";
bool loadObj(const std::string &filename, std::vector<float> &vertices,
             std::vector<unsigned int> &indices, std::vector<float> &normals,
             std::vector<float> &uvs)
{
  tinyobj::ObjReader reader;

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
  if (attrib.texcoords.size() == (attrib.vertices.size() / 3) * 2)
  {
    uvs = attrib.texcoords;
  }

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
  int sampling = 1;
  const unsigned int width = 512;
  const unsigned int height = 512;
  vector<float> vertices;
  vector<unsigned int> indices;
  vector<float> normal;
  vector<float> uvs;

  if (!loadObj(objname, vertices, indices, normal, uvs))
  {
    exit(EXIT_FAILURE);
  }
  Image img(width, height);
  vec3f camPos(0, 30, 89);
  vec3f lookAt(0, 30, 0);
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
  unsigned int index[] = {0, 2, 3, 0, 1, 2, 0, 7, 4, 0, 3, 7, 3, 2, 6, 3, 6, 7, 1, 5, 6, 1, 6, 2, 0, 5, 1, 0, 4, 5, 7, 6, 4, 6, 5, 4};
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
  float vertex10[24];
  for (int i = 0; i < 8; ++i)
  {
    vertex10[3 * i] = 90 * vertex2[3 * i];
    vertex10[3 * i + 1] = 90 * vertex2[3 * i + 1] + 30;
    vertex10[3 * i + 2] = 90 * vertex2[3 * i + 2];
  }
  // for (int i = 0; i < 12; i++)
  // {
  //   swap(index[3 * i], index[3 * i + 2]);
  // }
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
  // Polygon poly(36, vertex2, index, MaterialType::Diffuse, bsdf[1]);
  Polygon poly1(36, vertex10, index, MaterialType::Diffuse, bsdf[0]);
  Polygon poly3(indices.size(), vertices.data(), indices.data(), MaterialType::Diffuse, bsdf[5]);
  Scene scene(poly1);
  scene.addPolygonBVH(poly3);

  float vert[] = {4, 7.5, 4, -4, 7.5, 4, -4, 7.5, -4, 4, 7.5, -4};
  for (int i = 0; i < 4; ++i)
  {
    vert[i * 3] = 10 * vert[i * 3];
    vert[i * 3 + 1] = vert[i * 3 + 1] + 100;
    vert[i * 3 + 2] = 10 * vert[i * 3 + 2];
  }
  unsigned int idx[] = {0, 1, 2, 0, 2, 3};
  Polygon poly2(6, vert, idx, MaterialType::Emission, bsdf[1]);
  scene.addPolygonBVH(poly2);
  vector<int> index3(std::begin(index), std::end(index));
  vector<vec3f> vertexlight = {vec3f(1, 0, 1), vec3f(1, 0, -1), vec3f(-1, 0, 1), vec3f(-1, 0, -1)};
  vector<int> indexlight = {0, 1, 2, 1, 2, 3};

  for (int i = 0; i < vertexlight.size(); i++)
  {
    vertexlight[i] = MoveMat(vec3f(0, 7.8, 0)) * ScaleMat(vec3f(4)) * vertexlight[i];
  }
  // scene.addPolygon(&poly);

  for (int i = 0; i < vertex1.size(); i++)
  {
    vertex1[i] = ScaleMat(vec3f(4.0)) * MoveMat(vec3f(0, -8, 0)) * vertex1[i];
  }
  vector<int> index4 = {1, 2, 5, 2, 5, 6};
  // scene.polygon(vertex1, index1, vec3f(0.9), MaterialType::Diffuse, bsdf[0]);
  // scene.polygon(vertex1, index2, vec3f(0), MaterialType::Mirror, bsdf[4]);
  // scene.SphereAdd(MoveMat(vec3f(0, -4, -6)) * vec3f(-5, 0, 0), 2.0f, MaterialType::Diffuse, bsdf[4]);
  // scene.SphereAdd(MoveMat(vec3f(0, -4, -6)) * vec3f(5, 0, 0), 2.0, MaterialType::Diffuse, bsdf[2]);
  // scene.SphereAdd(MoveMat(vec3f(0, -4, -6)) * vec3f(0), 2.0f, MaterialType::Diffuse, bsdf[3]);
  // scene.SphereAdd(vec3f(2, -2, -2), 2.0, MaterialType::Glass, bsdf[5]);
  // scene.RectangleAdd(MoveMat(vec3f(0, 8, 0)) * vec3f(0), vec3f(0, -1, 0), MaterialType::Diffuse, bsdf[0]);
  cout << "Scene Set Complete" << endl;
  cout << "Pathtracing start" << endl;
  scene.Build();
  int i = 0;
  // AABB box(vec3f(-1), vec3f(1));
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
        // samplecolor = samplecolor + normalCheck(r, scene);
        // vec3f rinv(1.0f / r.direction[0], 1 / r.direction[1], 1 / r.direction[2]);
        // int dirInvSign[3];
        // for (int i = 0; i < 3; i++)
        // {
        //   dirInvSign[i] = rinv[i] > 0 ? 0 : 1;
        // }
        // if (box.intersect(r, rinv, dirInvSign))
        // {
        //   samplecolor = samplecolor + vec3f(1);
        // }
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