#ifndef _POLYGON_H
#define _POLYGON_H
#include <iostream>
#include <cassert>
#include <array>
#include "vec3.h"
#include "BSDF.h"
#include "geometry.h"

class Polygon
{
public:
    float *vertex;
    unsigned int *index;
    unsigned int nVertex;
    BSDF *bsdf;
    MaterialType material;

    Polygon(unsigned int nvertex, float *vert, unsigned int *inde, MaterialType mater, BSDF *inbsdf) : nVertex(nvertex), vertex(vert), index(inde), material(mater), bsdf(inbsdf) {}

    std::array<unsigned int, 3> getIndex(unsigned int FaceID)
    {
        assert(FaceID <= nFaces());
        return {index[3 * FaceID], index[3 * FaceID + 1], index[3 * FaceID + 2]};
    }

    vec3f getVertex(unsigned int ind)
    {
        return vec3f(vertex[3 * ind], vertex[3 * ind + 1], vertex[3 * ind + 2]);
    }

    unsigned int nFaces() const
    {
        return nVertex / 3;
    }

    MaterialType getMaterial() const
    {
        return material;
    }

    BSDF *getBSDF() const
    {
        return bsdf;
    }
};

#endif