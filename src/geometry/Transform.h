#pragma once

#include "Object.h"

class Translate : public Object {
    public:
        Translate(shared_ptr<Object> object, const Vec3& offset);
        virtual bool hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const override;
        virtual AABB bbox() const override;

    private:
        shared_ptr<Object> object;
        Vec3 offset;
};

class RotateY : public Object {
    public:
        RotateY(shared_ptr<Object> object, float theta);
        virtual bool hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const override;
        virtual AABB bbox() const override;

    private:
        shared_ptr<Object> object;
        double sinTheta, cosTheta;
};