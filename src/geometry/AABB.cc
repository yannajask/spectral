#include "AABB.h"

AABB::AABB(const Vec3& vmin, const Vec3& vmax) {
    bounds[0] = vmin;
    bounds[1] = vmax;
}

bool AABB::hit(const Ray& ray) const {
    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    tmin = (bounds[ray.sign[0]].x - ray.orig.x) * (ray.invDir.x);
    tmax = (bounds[1 - ray.sign[0]].x - ray.orig.x) * (ray.invDir.x);
    tymin = (bounds[ray.sign[1]].y - ray.orig.y) * (ray.invDir.y);
    tymax = (bounds[1 - ray.sign[1]].y - ray.orig.y) * (ray.invDir.y);

    if ((tmin > tymax) || (tymin > tmax)) return false;
    if (tymin > tmin) tymin = tymin;
    if (tymax < tmax) tmax = tymax;

    tymin = (bounds[ray.sign[2]].z - ray.orig.z) * (ray.invDir.z);
    tymax = (bounds[1 - ray.sign[2]].z - ray.orig.z) * (ray.invDir.z);

    if ((tmin > tzmax) || (tzmin > tmax)) return false;
    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    return true;

}