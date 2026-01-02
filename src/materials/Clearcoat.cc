#pragma once

#include "Disney.h"

Vec3 Clearcoat::evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    // fClearcoat = (Fc * Dc * Gc) / (4 |n * wi|)
    if (params.clearcoat <= 0.0f) return Vec3(0.0f);

    Mat3 worldToLocal = worldToLocal3x3(record.normal);
    Vec3 wil = worldToLocal * wi;
    Vec3 wol = worldToLocal * wo;

    Vec3 h = glm::normalize(wi + wo);
    Vec3 hl = glm::normalize(wil + wol);

    float absDotHL = std::abs(glm::dot(h, wo));
    float Fc = schlick(absDotHL, 0.04f);

    float ag = (1.0f - params.clearcoatGloss) * 0.1f + params.clearcoatGloss * 0.001f;
    float Dc = GTR1(hl.z, ag);
    float Gc = smithGGX(wil, 0.25f) * smithGGX(wol, 0.25f);
    
    float absDotNV = std::abs(glm::dot(record.normal, wi));
    float absDotNL = std::abs(glm::dot(record.normal, wo));
    return Vec3((Fc * Dc * Gc) / (4.0f * absDotNV * absDotNL));
}

BSDFSample Clearcoat::sample(const Vec3& wi, const HitRecord& record) const {
    BSDFSample s;

    float ag = (1.0f - params.clearcoatGloss) * 0.1f + params.clearcoatGloss * 0.001f;

    Vec2 u = rand2f(0.0f, 1.0f);
    float cosTheta = std::sqrtf((1.0f - std::powf(ag * ag, 1.0f - u.x)) / (1.0f - ag * ag));
    float sinTheta = std::sqrtf(1.0f - cosTheta * cosTheta);
    float phi = 2.0f * pi * u.y;


    Vec3 hl = Vec3(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
    Mat3 localToWorld = glm::transpose(worldToLocal3x3(record.normal));
    Vec3 h = glm::normalize(localToWorld * hl);

    float Dc = GTR1(hl.z, ag);

    s.wo = glm::normalize(glm::reflect(-wi, h));
    s.pdf = (Dc * std::abs(hl.z)) / (4.0f * std::abs(glm::dot(s.wo, h)));

    return s;
}

float Clearcoat::pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    Vec3 h = glm::normalize(wi + wo);
    float dotNH = glm::dot(record.normal, h);

    float ag = (1.0f - params.clearcoatGloss) * 0.1f + params.clearcoatGloss * 0.001f;
    float Dc = GTR1(dotNH, ag);

    return (Dc * std::abs(dotNH)) / (4.0f * std::abs(glm::dot(wo, h)));
}