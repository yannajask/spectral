#pragma once

#include "../utils.h"
#include "BSDF.h"

struct HitRecord;

class Material {
    public:
        virtual ~Material() = default;
        virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const = 0;
        virtual BSDFSample sample(const Vec3& wi, const HitRecord& record) const = 0;
        virtual float pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const = 0;
};