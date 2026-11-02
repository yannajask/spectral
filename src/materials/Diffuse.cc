#include "Disney.h"
#include "../geometry/HitRecord.h"

Vec3 Diffuse::evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    // fDiffuse = (1 - subsurface) * fBaseDiffuse + subsurface * fSubsurface
    float dotNV = glm::dot(record.normal, wi);
    float dotNL = glm::dot(record.normal, wo);

    if (dotNV <= 0.0f || dotNL <= 0.0f) return Vec3(0.0f);

    Vec3 h = glm::normalize(wi + wo);
    float absDotNV = std::abs(dotNV);
    float absDotNL = std::abs(dotNL);
    float dotHL = glm::dot(h, wo);

    float FD90 = 0.5f + 2.0f * params.roughness * (dotHL * dotHL);
    float FDi = schlickDiffuse(absDotNV, FD90);
    float FDo = schlickDiffuse(absDotNL, FD90);

    Vec3 fBaseDiffuse = (params.baseColour / pi) * FDi * FDo * absDotNL;

    if (params.subsurface > 0.0f) {
        float Fss90 = params.roughness * (dotHL * dotHL);
        float Fssi = schlickDiffuse(absDotNV, Fss90);
        float Fsso = schlickDiffuse(absDotNL, Fss90);

        float lsLaw = (1.0f / (absDotNV + absDotNL)) - 0.5f;
        Vec3 fSubsurface = (1.25f * params.baseColour / pi) * (Fssi * Fsso * lsLaw + 0.5f) * absDotNL;

        return glm::mix(fBaseDiffuse, fSubsurface, params.subsurface);
    }
    
    return fBaseDiffuse;
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