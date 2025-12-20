#include "Sphere.h"

Sphere::Sphere(const Vec3& center, float radius) : center(center), radius(radius) {
    _bbox = AABB(center - radius, center + radius);
}

bool Sphere::hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const {
    Vec3 oc = center - ray.orig;
    float a = glm::length2(ray.dir);
    float h = glm::dot(ray.dir, oc);
    float c = glm::length2(oc) - (radius * radius);
    float discriminant = h * h - a * c;

    if (discriminant < 0.0f) return false;

    float sqrtd = std::sqrt(discriminant);

    float root = (h - sqrtd) / a;
    if (root < tmin || root > tmax) {
        root = (h + sqrtd) / a;
        if (root < tmin || root > tmax) return false;
    }

    record.t = root;
    record.p = ray.at(root);
    Vec3 outwardNormal = (record.p - center) / radius;
    record.setFaceNormal(ray, outwardNormal);

    return true;
}

AABB Sphere::bbox() const {
    return _bbox;
}