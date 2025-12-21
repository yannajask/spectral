#pragma once

#include "Object.h"
#include "AABB.h"

class Triangle : public Object {
    public:
        Triangle(const Vec3& a, const Vec3& b, const Vec3& c, shared_ptr<Material> mat);

        virtual bool hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const override;

        virtual AABB bbox() const override;

    private:
        Vec3 v0, v1, v2;
        shared_ptr<Material> mat;
};