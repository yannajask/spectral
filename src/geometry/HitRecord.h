#pragma once

#include "../utils.h"
#include "../materials/Material.h"

class Material;

struct HitRecord {
    float t;
    Vec3 p;
    Vec3 normal;
    bool frontFace;

    float u, v;
    shared_ptr<Material> mat;

    void setFaceNormal(const Ray& ray, const Vec3& outwardNormal) {
        frontFace = glm::dot(ray.dir, outwardNormal) < 0.0f;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
};