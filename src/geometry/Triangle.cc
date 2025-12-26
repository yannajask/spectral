#include "Triangle.h"

// idk how needed this is, but will keep for standalone triangles
Triangle::Triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, shared_ptr<Material> mat)
     : v0(v0), v1(v1), v2(v2), mat(mat) {
    Vec3 faceNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
    n0 = n1 = n2 = faceNormal;

    Vec3 vmin = glm::min(v0, v1, v2);
    Vec3 vmax = glm::max(v0, v1, v2);
    _bbox = AABB(vmin, vmax);
}

Triangle::Triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2,
                   const Vec2& t0, const Vec2& t1, const Vec2& t2,
                   const Vec3& n0, const Vec3& n1, const Vec3& n2,
                   shared_ptr<Material> mat)
     : v0(v0), v1(v1), v2(v2), t0(t0), t1(t1), t2(t2), n0(n0), n1(n1), n2(n2), mat(mat) {
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
    record.p = ray.at(t);
    record.mat = mat;

    record.u = t0.x + u * (t1.x - t0.x) + v * (t2.x - t0.x);
    record.v = t0.y + u * (t1.y - t0.y) + v * (t2.y - t0.y);

    Vec3 outwardNormal = glm::normalize(n0 + u * (n1 - n0) + v * (n2 - n0));
    record.setFaceNormal(ray, outwardNormal);

    return true;
}

AABB Triangle::bbox() const {
    return _bbox;
}