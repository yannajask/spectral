#pragma once

#include "../utils.h"
#include "Object.h"
#include "AABB.h"
#include "HitRecord.h"

class BVH {
    public:
        unsigned int root;

        BVH(std::vector<shared_ptr<Object>>& objects);

        bool hit(const Ray& ray, float tmin, float tmax, HitRecord& record, unsigned int idx) const;

        AABB bbox() const {
            return nodes[root].bbox;
        }

    private:
        struct BVHNode {
            AABB bbox;
            unsigned int left, right;  // index of child nodes
            unsigned int first;        // index of first object (if leaf node)
            unsigned int count;        // number of objects within leaf node

            bool isLeaf() const { return count > 0; }
        };

        std::vector<BVHNode> nodes;
        std::vector<shared_ptr<Object>> objects;

        unsigned int build(size_t start, size_t end);
};
