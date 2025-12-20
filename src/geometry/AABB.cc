#include "AABB.h"

AABB::AABB() {
    bounds[0] = Vec3( infinity,  infinity,  infinity);
    bounds[1] = Vec3(-infinity, -infinity, -infinity);
}

AABB::AABB(const Vec3& vmin, const Vec3& vmax) {
    bounds[0] = vmin;
    bounds[1] = vmax;
}

AABB::AABB(const AABB& box1, const AABB& box2) {
    bounds[0] = Vec3(
        box1.min().x < box2.min().x ? box1.min().x : box2.min().x,
        box1.min().y < box2.min().y ? box1.min().y : box2.min().y,
        box1.min().z < box2.min().z ? box1.min().z : box2.min().z
    );
    bounds[1] = Vec3(
        box1.max().x > box2.max().x ? box1.max().x : box2.max().x,
        box1.max().y > box2.max().y ? box1.max().y : box2.max().y,
        box1.max().z > box2.max().z ? box1.max().z : box2.max().z
    );
}

bool AABB::hit(const Ray& ray, float tmin, float tmax) const {
    float txmin, txmax, tymin, tymax, tzmin, tzmax;

    txmin = (bounds[ray.sign[0]].x - ray.orig.x) * (ray.invDir.x);
    txmax = (bounds[1 - ray.sign[0]].x - ray.orig.x) * (ray.invDir.x);
    tymin = (bounds[ray.sign[1]].y - ray.orig.y) * (ray.invDir.y);
    tymax = (bounds[1 - ray.sign[1]].y - ray.orig.y) * (ray.invDir.y);

    if ((txmin > tymax) || (tymin > txmax)) return false;
    if (tymin > txmin) txmin = tymin;
    if (tymax < txmax) txmax = tymax;

    tzmin = (bounds[ray.sign[2]].z - ray.orig.z) * (ray.invDir.z);
    tzmax = (bounds[1 - ray.sign[2]].z - ray.orig.z) * (ray.invDir.z);

    if ((txmin > tzmax) || (tzmin > txmax)) return false;
    if (tzmin > txmin) txmin = tzmin;
    if (tzmax < txmax) txmax = tzmax;

    return (txmin <= tmax && txmax >= tmin);
}

void AABB::add(const AABB& other) {
    bounds[0].x = std::min(bounds[0].x, other.bounds[0].x);
    bounds[0].y = std::min(bounds[0].y, other.bounds[0].y);
    bounds[0].z = std::min(bounds[0].z, other.bounds[0].z);

    bounds[1].x = std::max(bounds[1].x, other.bounds[1].x);
    bounds[1].y = std::max(bounds[1].y, other.bounds[1].y);
    bounds[1].z = std::max(bounds[1].z, other.bounds[1].z);
}
