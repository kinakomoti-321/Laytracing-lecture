#ifndef _SIMPLE_H
#define _SIMPLE_H

#include <vector>
#include <numeric>
#include "triangle.h"
#include "BVH/AABB.h"
class SimpleBVH
{
private:
    std::vector<Triangle> primitives;

    struct BVHNode
    {
        AABB bbox;
        uint32_t primIndicesOffset;
        uint16_t nPrimitives;
        uint8_t axis;
        BVHNode *child[2];
    };

    struct BVHstatistics
    {
        int nNode{0};
        int nInternalNodes{0};
        int nLeafNodes{0};
    };

    BVHNode *root;
    BVHstatistics stats;

    BVHNode *createLeafNode(BVHNode *node, const AABB &bbox, int primIndicesOffset, int nPrimitives)
    {
        node->bbox = bbox;
        node->primIndicesOffset = primIndicesOffset;
        node->nPrimitives = nPrimitives;
        node->child[0] = nullptr;
        node->child[1] = nullptr;
        stats.nLeafNodes++;
        return node;
    }

    BVHNode *buildBVHNode(int primStart, int primEnd)
    {
        BVHNode *node = new BVHNode;
        AABB bbox;

        for (int i = primStart; i < primEnd; i++)
        {
            bbox = mergeAABB(bbox, primitives[i].calcAABB());
        }

        const int nPrim = primEnd - primStart;
        if (nPrim <= 2)
        {
            return createLeafNode(node, bbox, primStart, nPrim);
        }

        AABB splitAABB;
        for (int i = primStart; i < primEnd; i++)
        {
            splitAABB = mergeAABB(splitAABB, primitives[i].calcAABB().center());
        }

        const int splitAxis = splitAABB.longestAxis();

        const int splitIdx = primStart + nPrim / 2;
        std::nth_element(primitives.begin() + primStart, primitives.begin() + splitIdx, primitives.begin() + primEnd, [&](auto &prim1, auto &prim2) { return prim1.calcAABB().center()[splitAxis] < prim2.calcAABB().center()[splitAxis]; });

        // 分割が失敗した場合は葉ノードを作成
        if (splitIdx == primStart || splitIdx == primEnd)
        {
            std::cout << "splitting failed" << std::endl;
            std::cout << "nPrimitives: " << nPrim << std::endl;
            std::cout << "splitAxis: " << splitAxis << std::endl;
            // std::cout << "splitPos: " << splitPos << std::endl;
            std::cout << "primStart: " << primStart << std::endl;
            std::cout << "splitIdx: " << splitIdx << std::endl;
            std::cout << "primEnd: " << primEnd << std::endl;
            std::cout << std::endl;
            // 葉ノードの作成
            return createLeafNode(node, bbox, primStart, nPrim);
        }

        node->bbox = bbox;
        node->primIndicesOffset = primStart;
        node->axis = splitAxis;

        node->child[0] = buildBVHNode(primStart, splitIdx);
        node->child[1] = buildBVHNode(splitIdx, primEnd);
        stats.nInternalNodes++;
        // cout << bbox << endl;
        // for (int i = 0; i < primitives.size(); ++i)
        // {
        //     primitives[i].faceCheck();
        // }
        // cout << endl;
        return node;
    }

    void deleteBVHNode(BVHNode *node)
    {
        if (node->child[0])
            deleteBVHNode(node->child[0]);
        if (node->child[1])
            deleteBVHNode(node->child[1]);
        delete node;
    }

    bool intersectNode(const BVHNode *node, Ray &ray, const vec3f &dirInv, const int dirInvSign[3], IntersectInfo &info)
    {
        bool hit = false;
        if (node->bbox.intersect(ray, dirInv, dirInvSign))
        {
            if (node->child[0] == nullptr && node->child[1] == nullptr)
            {
                const int primEnd = node->primIndicesOffset + node->nPrimitives;
                for (int i = node->primIndicesOffset; i < primEnd; ++i)
                {
                    IntersectInfo sect;
                    if (primitives[i].hit(ray, sect))
                    {
                        if (sect.distance < info.distance)
                        {
                            info = sect;
                        }
                        hit = true;
                    }
                }
            }
            else
            {
                hit |= intersectNode(node->child[dirInvSign[node->axis]], ray, dirInv, dirInvSign, info);
                hit |= intersectNode(node->child[1 - dirInvSign[node->axis]], ray, dirInv, dirInvSign, info);
            }
        }

        return hit;
    }

public:
    SimpleBVH(Polygon &polygon)
    {
        for (unsigned int f = 0; f < polygon.nFaces(); ++f)
        {
            primitives.emplace_back(&polygon, f);
        }
    }

    void AddPolygon(Polygon &polygon)
    {
        for (unsigned int f = 0; f < polygon.nFaces(); ++f)
        {
            primitives.emplace_back(&polygon, f);
        }
    }

    ~SimpleBVH()
    {
        if (root)
        {
            deleteBVHNode(root);
        }
    }

    void buildBVH()
    {
        root = buildBVHNode(0, primitives.size());
        stats.nNode = stats.nInternalNodes + stats.nLeafNodes;
    }

    AABB rootAABB() const
    {
        if (root)
        {
            return root->bbox;
        }
        else
        {
            return AABB();
        }
    }

    bool intersect(Ray &ray, IntersectInfo &info)
    {
        const vec3f dirInv(1.0f / ray.direction[0], 1.0f / ray.direction[1], 1.0f / ray.direction[2]);
        int dirInvSign[3];
        for (int i = 0; i < 3; ++i)
        {
            dirInvSign[i] = dirInv[i] > 0 ? 0 : 1;
        }
        return intersectNode(root, ray, dirInv, dirInvSign, info);
    }
};
#endif