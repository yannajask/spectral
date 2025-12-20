#include "BVH.h"

BVH::BVH(std::vector<shared_ptr<Object>>& objects) : objects(objects) {
    if (!objects.empty()) {
        nodes.reserve(objects.size() * 2 - 1);
        root = build(0, objects.size());
    }
}

bool BVH::hit(const Ray& ray, float tmin, float tmax, HitRecord& record, unsigned int idx) const {
    const BVHNode& node = nodes[idx];

    if (!node.bbox.hit(ray, tmin, tmax)) return false;

    if (node.isLeaf()) {
        bool hit = false;
        for (unsigned int i = 0; i < node.count; i++) {
            if (objects[node.first + i]->hit(ray, tmin, tmax, record)) {
                hit = true;
                tmax = record.t;
            }
        }

        return hit;
    } else {
        bool hitLeft = hit(ray, tmin, tmax, record, node.left);
        bool hitRight = hit(ray, tmin, (hitLeft ? record.t : tmax), record, node.right);
        return hitLeft || hitRight;
    }
}

unsigned int BVH::build(size_t start, size_t end) {
    //nodes.emplace_back();
    //unsigned int idx = nodes.size() - 1;

    AABB bbox = objects[start]->bbox();
    for (size_t i = start + 1; i < end; i++) {
        bbox = AABB(bbox, objects[i]->bbox());
    }
    
    //nodes[idx].bbox = bbox;

    size_t count = end - start;
    BVHNode node{};
    node.bbox = bbox;

    if (count <= 4) {
        node.first = static_cast<unsigned int>(start);
        node.count = static_cast<unsigned int>(count);
        node.left = node.right = 0;
        nodes.push_back(node);
        return static_cast<unsigned int>(nodes.size() - 1);
    } else {
        node.count = 0;

        int axis = bbox.longestAxis();
        auto comparator = [axis](const shared_ptr<Object>& a, const shared_ptr<Object>& b) {
            return a->bbox().center()[axis] < b->bbox().center()[axis];
        };
        size_t mid = start + (count / 2);

        std::nth_element(objects.begin() + start, objects.begin() + mid, objects.begin() + end, comparator);

        nodes.push_back(node);
        unsigned int idx = static_cast<unsigned int>(nodes.size() - 1);

        unsigned int leftidx = build(start, mid);
        unsigned int rightidx = build(mid, end);

        nodes[idx].left = leftidx;
        nodes[idx].right = rightidx;

        return idx;
    }
}
