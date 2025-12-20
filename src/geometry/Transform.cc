#include "Transform.h"

// translations

Translate::Translate(shared_ptr<Object> object, const Vec3& offset) : object(object), offset(offset) {
    _bbox = object->bbox() + offset;
}

bool Translate::hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const {
    Ray offsetRay(ray.orig - offset, ray.dir);
    if (!object->hit(offsetRay, tmin, tmax, record)) return false;
    record.p += offset;
    return true;
}

AABB Translate::bbox() const {
    return _bbox;
}


// rotations (y axis)

RotateY::RotateY(shared_ptr<Object> object, float theta)
     : object(object), sinTheta(std::sin(radians(theta))), cosTheta(std::cos(radians(theta))) {
    AABB bbox = object->bbox();
    Vec3 min(infinity);
    Vec3 max(-infinity);

    for (unsigned int i = 0; i < 2; i++) {
        for (unsigned int j = 0; j < 2; j++) {
            for (unsigned int k = 0; k < 2; k++) {
                float x = i ? bbox.max().x : bbox.min().x;
                float y = j ? bbox.max().y : bbox.min().y;
                float z = k ? bbox.max().z : bbox.min().z;

                float newx = cosTheta * x + sinTheta * z;
                float newz = -sinTheta * x+ cosTheta * z;
                Vec3 tester(newx, y, newz);

                min = glm::min(min, tester);
                max = glm::max(max, tester);
            }
        }
    }

    _bbox = AABB(min, max);
}

// to do: this is uglyyyyy so prob fix
bool RotateY::hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const {
    Vec3 orig = ray.orig;
    Vec3 dir = ray.dir;

    orig.x = cosTheta * ray.orig.x - sinTheta * ray.orig.z;
    orig.z = sinTheta * ray.orig.x + cosTheta * ray.orig.z;

    dir.x = cosTheta * ray.dir.x - sinTheta * ray.dir.z;
    dir.z = sinTheta * ray.dir.x + cosTheta * ray.dir.z;

    Ray rotatedRay(orig, dir);
    if (!object->hit(rotatedRay, tmin, tmax, record)) return false;

    Vec3 p = record.p;
    record.p.x = cosTheta * p.x + sinTheta * p.z;
    record.p.z = -sinTheta * p.x + cosTheta * p.z;

    Vec3 normal = record.normal;
    record.normal.x = cosTheta * normal.x + sinTheta * normal.z;
    record.normal.z = -sinTheta * normal.x + cosTheta * normal.z;

    return true;
}

AABB RotateY::bbox() const {
    return _bbox;
}
