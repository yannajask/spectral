#pragma once

#include "../utils.h"
#include "../geometry/HitRecord.h"

struct HitRecord;

class Material {
    public:
        virtual ~Material() = default;
        virtual bool scatter(const Ray &ray, const HitRecord& record, Vec3& attenuation, Ray& scattered) const = 0;
};