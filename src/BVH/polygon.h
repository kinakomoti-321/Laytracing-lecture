#ifndef _POLYGON_H_
#define _POLYGON_H_

#include "vec3.h"
#include <iostream>
#include <array>

struct Polygon
{
    unsigned int nVertex;
    float *vertex;
    unsigned int *index;

    Polygon(unsigned int nVertexs, float *vertexs, unsigned int *indexs) : nVertex(nVertexs), vertex(vertexs), index(indexs){};

    vec3f getVertex(unsigned int vertInt) const
    {
        return vec3f(vertex[3 * vertInt], vertex[3 * vertInt + 1], vertex[3 * vertInt + 2]);
    }

    std::array<unsigned int, 3> getIndex(unsigned int ID) const
    {
        return {index[3 * ID], index[3 * ID + 1], index[3 * ID + 2]};
    }

    unsigned int nFace() const { return nVertex / 3; }
};

#endif