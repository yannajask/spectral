#include "Disney.h"
#include "../geometry/HitRecord.h"

Vec3 Metal::evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    // fMetal = (Fm * Dm * Gm) / (4 |n * wi|)
    float eta = record.frontFace ? (1.0f / params.ior) : params.ior;

    float dotNV = glm::dot(record.normal, wi);
    float dotNL = glm::dot(record.normal, wo);

    if (dotNV <= 0.0f || dotNL <= 0.0f) return Vec3(0.0f);

    Mat3 worldToLocal = worldToLocal3x3(record.normal);
    Vec3 wil = worldToLocal * wi;
    Vec3 wol = worldToLocal * wo;

    Vec3 h = glm::normalize(wi + wo);
    Vec3 hl = glm::normalize(wil + wol);

    float absDotHL = std::abs(glm::dot(h, wo));
    float luminance = glm::dot(Vec3(0.3f, 0.6f, 1.0f), params.baseColour);
    Vec3 cTint = (luminance > 0.0f) ? params.baseColour / luminance : Vec3(1.0f);
    Vec3 Ks = (1.0f - params.specularTint) + params.specularTint * cTint;
    float R0 = ((eta - 1.0f) * (eta - 1.0f)) / ((eta + 1.0f) * (eta + 1.0f));
    Vec3 C0 = params.specular * R0 * (1.0f - params.metallic) * Ks + params.metallic * params.baseColour;
    Vec3 Fm = schlick(absDotHL, C0);

    float aspect = std::sqrt(1.0f - 0.9f * params.anisotropic);
    float roughness2 = params.roughness * params.roughness;
    float ax = std::max(0.0001f, roughness2 / aspect);
    float ay = std::max(0.0001f, roughness2 * aspect);
    float Dm = anisotropicGTR2(hl, ax, ay);

    float Gm = anisotropicSmithGGX(wil, ax, ay) * anisotropicSmithGGX(wol, ax, ay);

    return (Fm * Dm * Gm) / (4.0f * std::abs(dotNV) * std::abs(dotNL));
}

BSDFSample Metal::sample(const Vec3& wi, const HitRecord& record) const {
    BSDFSample s;
    float aspect = std::sqrt(1.0f - 0.9f * params.anisotropic);
    float roughness2 = params.roughness * params.roughness;
    float ax = std::max(0.0001f, roughness2 / aspect);
    float ay = std::max(0.0001f, roughness2 * aspect);

    Mat3 worldToLocal = worldToLocal3x3(record.normal);
    Vec3 wil = worldToLocal * wi;

    Vec3 vh = glm::normalize(Vec3(ax * wil.x, ay * wil.y, wil.z));
    Vec3 T1 = (vh.z < 0.9999f) ? glm::normalize(glm::cross(Vec3(0.0f, 0.0f, 1.0f), vh)) : Vec3(1.0f, 0.0f, 0.0f);
    Vec3 T2 = glm::cross(vh, T1);

    Vec2 u = rand2f(0.0f, 1.0f);
    float r = sqrt(u.x);
    float phi = 2.0f * pi * u.y;
    float t1 = r * std::cos(phi);
    float t2 = r * std::sin(phi);
    float c = 0.5f * (1.0f + vh.z);
    t2 = (1.0f - c) * std::sqrt(1.0f - t1 * t1) + c * t2;

    Vec3 nh = t1 * T1 + t2 * T2 + std::sqrt(std::fmax(0.0f, 1.0f - t1 * t1 - t2 * t2)) * vh;
    Vec3 hl = glm::normalize(Vec3(ax * nh.x, ay * nh.y, std::fmax(0.0f, nh.z)));

    float Dm = anisotropicGTR2(hl, ax, ay);
    float Gwi = anisotropicSmithGGX(wil, ax, ay);

    s.pdf = (Dm * Gwi) / (4.0f * std::abs(glm::dot(record.normal, wi)));

    Vec3 wol = glm::normalize(glm::reflect(-wil, hl));
    s.wo = glm::transpose(worldToLocal) * wol;

    return s;
}

float Metal::pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    Mat3 worldToLocal = worldToLocal3x3(record.normal);
    Vec3 wil = worldToLocal * wi;

    Vec3 h = glm::normalize(wi + wo);
    Vec3 hl = worldToLocal * h;

    float aspect = std::sqrt(1.0f - 0.9f * params.anisotropic);
    float roughness2 = params.roughness * params.roughness;
    float ax = std::max(0.0001f, roughness2 / aspect);
    float ay = std::max(0.0001f, roughness2 * aspect);

    float Dm = anisotropicGTR2(hl, ax, ay);
    float Gwi = anisotropicSmithGGX(wil, ax, ay);

    return(Dm * Gwi) / (4.0f * std::abs(glm::dot(record.normal, wi)));
}