#include "Triangle.h"

Triangle::Triangle(const Vec3& a, const Vec3& b, const Vec3& c) : v0(a), v1(b), v2(c) {
    Vec3 vmin = glm::min(v0, v1, v2);
    Vec3 vmax = glm::max(v0, v1, v2);
    _bbox = AABB(vmin, vmax);
}

// moller-trumbore algorithm
bool Triangle::hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const {
    Vec3 e1 = v1 - v0;
    Vec3 e2 = v2 - v0;
    Vec3 pvec = glm::cross(ray.dir, e2);
    float determinant = glm::dot(e1, pvec);

    if (fabs(determinant) < 1e-6f) return false;

    float invDeterminant = 1.0f / determinant;

    Vec3 tvec = ray.orig - v0;
    float u = glm::dot(tvec, pvec) * invDeterminant;
    if (u < 0 || u > 1) return false;

    Vec3 qvec = glm::cross(tvec, e1);
    float v = glm::dot(ray.dir, qvec) * invDeterminant;
    if (v < 0 || u + v > 1) return false;

    float t = glm::dot(e2, qvec) * invDeterminant;
    if (t < tmin || t > tmax) return false;

    record.t = t;
    record.u = u;
    record.v = v;
    record.p = ray.at(t);
    Vec3 outwardNormal = glm::normalize(glm::cross(e1, e2));
    record.setFaceNormal(ray, outwardNormal);

    return true;
}

AABB Triangle::bbox() const {
    return _bbox;
}