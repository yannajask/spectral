#pragma once

#include "Object.h"
#include "AABB.h"

class Triangle : public Object {
    public:
        Triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, shared_ptr<Material> mat);

        Triangle(const Vec3& v0, const Vec3& v1, const Vec3& v2,
                 const Vec2& t0, const Vec2& t1, const Vec2& t2,
                 const Vec3& n0, const Vec3& n1, const Vec3& n2,
                 shared_ptr<Material> mat);

        virtual bool hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const override;

        virtual AABB bbox() const override;

    private:
        Vec3 v0, v1, v2;                                     // vertices
        Vec2 t0{0.0f, 0.0f}, t1{1.0f, 0.0f}, t2{0.0f, 1.0f}; // tex coords
        Vec3 n0, n1, n2;                                     // vertex normals
        shared_ptr<Material> mat;
};