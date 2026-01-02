#pragma once

#include "Disney.h"

Vec3 Diffuse::evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    // fDiffuse = (1 - subsurface) * fBaseDiffuse + subsurface * fSubsurface
    Vec3 h = glm::normalize(wi + wo);
    float absDotNV = std::abs(glm::dot(record.normal, wi));
    float absDotNL = std::abs(glm::dot(record.normal, wo));
    float dotHL = glm::dot(h, wo);

    float FD90 = 0.5f + 2.0f * params.roughness * (dotHL * dotHL);
    float FDi = schlick(absDotNV, FD90);
    float FDo = schlick(absDotNL, FD90);

    Vec3 fBaseDiffuse = (params.baseColour / pi) * FDi * FDo;

    float Fss90 = params.roughness * (dotHL * dotHL);
    float Fssi = schlick(absDotNV, Fss90);
    float Fsso = schlick(absDotNL, Fss90);

    float lsLaw = (1.0f / (absDotNV + absDotNL)) - 0.5f;
    Vec3 fSubsurface = (1.25f * params.baseColour / pi) * (Fssi * Fsso * lsLaw + 0.5f);

    return glm::mix(fBaseDiffuse, fSubsurface, params.subsurface);
}

BSDFSample Diffuse::sample(const Vec3& wi, const HitRecord& record) const {
    BSDFSample s;
    s.wo = rand3fcd(record.normal);
    s.pdf = glm::dot(record.normal, s.wo) / pi;
    return s;
}

float Diffuse::pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    return glm::dot(record.normal, wo) / pi;
}