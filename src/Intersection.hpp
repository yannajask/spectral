#pragma once

#include "Ray.hpp"

class Intersection {
    public:
        Vec3 p;
        Vec3 normal;
        float t;
        float u;
        float v;
        bool frontFace;

        void setFaceNormal(const Ray& ray, const Vec3& outwardNormal) {
            frontFace = glm::dot(ray.dir, outwardNormal) < 0.0f;
            normal = frontFace ? outwardNormal : -outwardNormal;
        }
};