#ifndef _POLYGON_H
#define _POLYGON_H
#include <iostream>
#include <cassert>
#include <array>

#include "../vec3.h"
#include "../BSDF.h"
#include "../geometry.h"
#include "../Picture/Texture.h"
#include "../intersect-info.h"

struct IntersectInfo;

class Polygon
{
public:
    float *vertex;
    unsigned int *index;
    unsigned int nVertex;
    float *uv = nullptr;
    float *normal = nullptr;
    Texture *tex = nullptr;
    BSDF *bsdf;
    MaterialType material;

    Polygon(unsigned int nvertex, float *vert, unsigned int *inde, MaterialType mater, BSDF *inbsdf) : nVertex(nvertex), vertex(vert), index(inde), material(mater), bsdf(inbsdf) {}
    Polygon(unsigned int nvertex, float *vert, unsigned int *inde, MaterialType mater, BSDF *inbsdf, float *UV, Texture *texture) : nVertex(nvertex), vertex(vert), index(inde), material(mater), bsdf(inbsdf), uv(UV), tex(texture) {}

    std::array<unsigned int, 3> getIndex(unsigned int FaceID)
    {
        assert(FaceID <= nFaces());
        return {index[3 * FaceID], index[3 * FaceID + 1], index[3 * FaceID + 2]};
    }

    vec3f getVertex(unsigned int ind)
    {
        return vec3f(vertex[3 * ind], vertex[3 * ind + 1], vertex[3 * ind + 2]);
    }
    vec3f getNormal(unsigned int ind)
    {
        return vec3f(normal[3 * ind], normal[3 * ind + 1], vertex[3 * ind + 2]);
    }
    std::pair<float, float> getUV(unsigned int ind)
    {
        return {uv[2 * ind], uv[2 * ind + 1]};
    }

    unsigned int nFaces() const
    {
        return nVertex / 3;
    }

    MaterialType getMaterial() const
    {
        return material;
    }

    vec3f getBSDF(RNGrandom &rng, const vec3f &wo, vec3f &wi, float &pdf, float u, float v)
    {
        if (hasTexture())
            bsdf->rho = tex->getTex(u, v);
        return bsdf->sampling(rng, wo, wi, pdf);
    }

    bool hasUV()
    {
        return uv != nullptr;
    }
    bool hasNormal()
    {
        return normal != nullptr;
    }
    bool hasTexture()
    {
        return tex != nullptr;
    }
};

#endif