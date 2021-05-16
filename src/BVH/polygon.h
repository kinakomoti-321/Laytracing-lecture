#ifndef _POLYGON_H_
#define _POLYGON_H_

#include "vec3.h"
#include <iostream>
#include <array>
#include <vector>

struct Polygon
{
    unsigned int nVertex;
    float *vertex;
    unsigned int *index;
    vec3f color;

    Polygon(unsigned int nVertexs, float *vertexs, unsigned int *indexs, vec3f &color) : nVertex(nVertexs), vertex(vertexs), index(indexs), color(color){};
    // Polygon(unsigned int nVertexs, vector<vec3f> &vertexs, unsigned int *indexs, vec3f &color, int delta) : nVertex(nVertexs), index(indexs), color(color)
    // {
    //     vector<float> v;
    //     for (int i = 0; i < vertexs.size(); ++i)
    //     {
    //         v.push_back(vertexs[i][0]);
    //         v.push_back(vertexs[i][1]);
    //         v.push_back(vertexs[i][2]);
    //     }

    //     vertex = v.data();
    // };

    vec3f getVertex(unsigned int vertInt) const
    {
        return vec3f(vertex[3 * vertInt], vertex[3 * vertInt + 1], vertex[3 * vertInt + 2]);
    }

    std::array<unsigned int, 3> getIndex(unsigned int ID) const
    {
        return {index[3 * ID], index[3 * ID + 1], index[3 * ID + 2]};
    }

    unsigned int nFaces() const { return nVertex / 3; }
};

#endif