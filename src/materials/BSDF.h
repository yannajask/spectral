#pragma once

#include "../utils.h"

struct HitRecord;

struct BSDFSample {
    Vec3 f{0.0f};
    Vec3 wo;
    float pdf;
};

class BSDF {
    public:
        virtual ~BSDF() = default;
        virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const = 0;
        virtual BSDFSample sample(const Vec3& wi, const HitRecord& record) const = 0;
        virtual float pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const = 0;
};