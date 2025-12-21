#pragma once

#include "Object.h"
#include "Triangle.h"
#include "BVH.h"
#include "AABB.h"

class Mesh : public Object {
    public:
        Mesh(const std::string& objPath, shared_ptr<Material> mat);

        void addTriangle(shared_ptr<Triangle> triangle);

        void addTriangle(const Vec3& a, const Vec3& b, const Vec3&c);

        virtual bool hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const override;

        virtual AABB bbox() const override;

    private:
        std::vector<shared_ptr<Object>> triangles;
        unique_ptr<BVH> bvh;
        shared_ptr<Material> mat;
};