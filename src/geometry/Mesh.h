#pragma once

#include "Object.h"
#include "Triangle.h"

class Mesh : public Object {
    public:
        explicit Mesh(const std::string& objPath);

        void addTriangle(const Triangle& triangle);

        void addTriangle(const Vec3& a, const Vec3& b, const Vec3&c);

        virtual bool hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const override;

    private:
        std::vector<Triangle> triangles;
};