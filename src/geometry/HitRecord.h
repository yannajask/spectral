#pragma once

#include "../utils.h"
#include "../materials/Material.h"

class Material;

struct HitRecord {
    float t;
    Vec3 p;
    Vec3 normal, geometricNormal;
    bool frontFace;

    float u, v;
    shared_ptr<Material> mat;

    void setFaceNormal(const Ray& ray, const Vec3& outwardNormal) {
        frontFace = glm::dot(ray.dir, outwardNormal) < 0.0f;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }

    void setGeoNormal(const Ray& ray, const Vec3& outwardGeoNormal) {
        frontFace = glm::dot(ray.dir, outwardGeoNormal) < 0.0f;
        geometricNormal = frontFace ? outwardGeoNormal : -outwardGeoNormal;
    }
};