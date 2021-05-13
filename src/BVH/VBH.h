#ifndef _BVH_H
#define _BVH_H

#include <vector>
#include <numeric>

#include "BVH/Triangle.h"

using namespace std;
class SimpleBVH
{
private:
    vector<Triangle> primitives;

    //BVHのノード
    struct BVHNode
    {
        AABB bbox;                  //ノードに入るAABB
        uint32_t primIndicesOffset; //primIndicesへのオフセット
        uint16_t nPrimitives;       //ノードに含まれるPrimitiveの数
        uint8_t axis;               //分割軸
        BVHNode *child[2];          //子ノードへのポインタ
    };

    struct BVHStatistics
    {
        int nNode{0};
        int nInternalNode{0};
        int nLeafNode{0};
    };

    BVHNode *root;
    BVHStatistics stats;
    // 葉ノードの作成
    BVHNode *createLeafNode(BVHNode *node, const AABB &bbox, int primIndicesOffset, int nPrimitives)
    {
        node->bbox = bbox;
        node->primIndicesOffset = primIndicesOffset;
        node->nPrimitives = nPrimitives;
        node->child[0] = nullptr;
        node->child[1] = nullptr;
        stats.nLeafNode++;
        return node;
    }

    //BVHノードの生成、再起的に行っていく
    BVHNode *buildBVHNode(int primStart, int primEnd)
    {
        BVHNode *node = new BVHNode;
        AABB bbox;

        //primstartからprimEndまでの三角形を包むAABBを作る
        for (int i = primStart; i < primEnd; ++i)
        {
            bbox = mergeAABB(bbox, primitives[i].calcAABB());
        }

        const int nPrims = primEnd - primStart;
        //nprimが４（つまり処理を行いたいポリゴンが４枚以下）の時葉ノードの作成を行う
        if (nPrims <= 4)
        {
            return createLeafNode(node, bbox, primStart, nPrims);
        }
        //それぞれのAABBの中点を囲むようなAABBを作成する（これじゃないとsplitがうまくいかないらしい）
        AABB splitAABB;
        for (int i = primStart; i < primEnd; ++i)
        {
            splitAABB = mergeAABB(splitAABB, primitives[i].calcAABB().center());
        }

        const int splitAxis = splitAABB.longestAxis();
        const float splitPos = splitAABB.center()[splitAxis];
        const int splitIdx = primStart + nPrims / 2;
        //nth_elemntは第二引数のイテレーターが示すものを基準として並び替えを行うもの
        std::nth_element(primitives.begin() + primStart, primitives.begin() + splitIdx, primitives.begin() + primEnd, [&](const auto &prim1, const auto &prim2) {
            return prim1.calcAABB().center()[splitAxis] < prim2.calcAABB().center()[splitAxis];
        });

        // 分割が失敗した場合は葉ノードを作成
        if (splitIdx == primStart || splitIdx == primEnd)
        {
            std::cout << "splitting failed" << std::endl;
            std::cout << "nPrimitives: " << nPrims << std::endl;
            std::cout << "splitAxis: " << splitAxis << std::endl;
            std::cout << "splitPos: " << splitPos << std::endl;
            std::cout << "primStart: " << primStart << std::endl;
            std::cout << "splitIdx: " << splitIdx << std::endl;
            std::cout << "primEnd: " << primEnd << std::endl;
            std::cout << std::endl;
            // 葉ノードの作成
            return createLeafNode(node, bbox, primStart, nPrims);
        }

        node->bbox = bbox;
        node->primIndicesOffset = primStart;
        node->axis = splitAxis;

        node->child[0] = buildBVHNode(primStart, splitIdx);
        node->child[1] = buildBVHNode(splitIdx, primEnd);

        stats.nInternalNode++;

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

    bool intersectNode(const BVHNode *node, const Ray &ray, const vec3f &dirInv, const int dirInvSign[3], IntersectInfo &info) const
    {
        bool hit = false;
        if (node->bbox.intersect(ray, dirInv, dirInvSign))
        {
            if (node->child[0] == nullptr && node->child[1] == nullptr)
            {
                const int primEnd = node->primIndicesOffset + node->nPrimitives;
                for (int i = node->primIndicesOffset; i < primEnd; ++i)
                {
                    IntersectInfo testinfo;
                    if (primitives[i].intersect(ray, testinfo))
                    {
                        hit = true;
                        if (testinfo.distance < info.distance)
                        {
                            info = testinfo;
                        }
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
    //Triangleを作成
    SimpleBVH(const Polygon &polygon)
    {
        for (unsigned int f = 0; f < polygon.nFace(); ++f)
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

        stats.nNode = stats.nInternalNode * stats.nLeafNode;
    }

    int nNode() const { return stats.nNode; }
    int nInternalNode() const { return stats.nInternalNode; }
    int nLeafNode() const { return stats.nLeafNode; }

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

    bool intersect(const Ray &ray, IntersectInfo &info)
    {
        const vec3f dirInv = 1.0f / ray.getdirection();
        int dirInvSign[3];
        for (int i = 0; i < 3; ++i)
        {
            dirInvSign[i] = dirInv[i] > 0 ? 0 : 1;
        }

        return intersectNode(root, ray, dirInv, dirInvSign, info);
    }
};

#endif