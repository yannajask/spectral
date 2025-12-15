#pragma once

#include "Object.h"

class Sphere : public Object {
    public:
        Sphere(const Vec3& center, float radius);

        virtual bool hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const override;
        
    private:
        Vec3 center;
        float radius;
};