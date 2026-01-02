#include "Disney.h"
#include "../geometry/HitRecord.h"

Vec3 Sheen::evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    // fSheen = cSheen (1 - |h * wo|)^5 *  |n * wo|
    if (params.sheen <= 0.0f) return Vec3(0.0f);

    Vec3 h = glm::normalize(wi + wo);
    float absDotNL = std::abs(glm::dot(record.normal, wo));
    float absDotHV = glm::dot(h, wi);
    float luminance = glm::dot(Vec3(0.3f, 0.6f, 1.0f), params.baseColour);
    Vec3 cTint = (luminance > 0.0f) ? params.baseColour / luminance : Vec3(1.0f);
    Vec3 cSheen = glm::mix(Vec3(1.0f), cTint, params.sheenTint);
    return cSheen * std::powf(1.0f - absDotHV, 5);
}

BSDFSample Sheen::sample(const Vec3& wi, const HitRecord& record) const {
    BSDFSample s;
    s.wo = rand3fcd(record.normal);
    s.pdf = glm::dot(record.normal, s.wo) / pi;
    return s;
}

float Sheen::pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    return glm::dot(record.normal, wo) / pi;
}