#pragma once

#include "../utils.h"
#include "../materials/Material.h"

class Material;

struct HitRecord {
    Vec3 p;
    Vec3 normal;
    float t;
    float u;
    float v;
    bool frontFace;
    shared_ptr<Material> mat;

    void setFaceNormal(const Ray& ray, const Vec3& outwardNormal) {
        frontFace = glm::dot(ray.dir, outwardNormal) < 0.0f;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
};