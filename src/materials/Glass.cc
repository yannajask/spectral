#pragma once

#include "Disney.h"

Vec3 Glass::evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    // fGlass = (baseColour * Fg * Dg * Gg) / (4 * |n * wi|) if reflect
    //        = (sqrt(baseColour)(1 - Fg) * Dg * Gg * |h * wo * h * wi|) / (|n * wi| * (h * wi + n * h * wi))
    float eta = record.frontFace ? (1.0f / params.ior) : params.ior;

    Mat3 worldToLocal = worldToLocal3x3(record.normal);
    Vec3 wil = worldToLocal * wi;
    Vec3 wol = worldToLocal * wo;

    float dotNGV = glm::dot(record.geometricNormal, wi);
    float dotNGL = glm::dot(record.geometricNormal, wo);
    bool reflect = (dotNGV * dotNGL > 0.0f);
    
    Vec3 h = (reflect) ? glm::normalize(wi + wo) : -glm::normalize(wi + eta * wo);
    Vec3 hl = glm::normalize(wil + wol);

    float dotHV = glm::dot(h, wi);
    float dotHL = glm::dot(h, wo);

    float Fg = fresnel(std::abs(dotHV), eta);

    float aspect = std::sqrt(1.0f - 0.9f * params.anisotropic);
    float roughness2 = params.roughness * params.roughness;
    float ax = std::max(0.0001f, roughness2 / aspect);
    float ay = std::max(0.0001f, roughness2 * aspect);
    float Dg = anisotropicGTR2(hl, ax, ay);
        
    float Gg = anisotropicSmithGGX(wil, ax, ay) * anisotropicSmithGGX(wol, ax, ay);

    float absDotNL = std::fabs(glm::dot(record.normal, wo));
    float absDotNV = std::fabs(glm::dot(record.normal, wi));
    if (reflect) {
        return (params.baseColour * Fg * Dg * Gg) / (4.0f * absDotNV);
    } else {
        float denom = dotHV + eta * dotHL;
        return (glm::sqrt(params.baseColour) * (1.0f - Fg) * Dg * Gg * std::abs(dotHV * dotHL)) / (absDotNV * denom * denom);
    }
}

BSDFSample Glass::sample(const Vec3& wi, const HitRecord& record) const {
    BSDFSample s;

    float eta = record.frontFace ? (1.0f / params.ior) : params.ior;

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
    float dotHV = glm::dot(hl, wil);

    float Fg = fresnel(std::abs(dotHV), eta);
    float Dg = anisotropicGTR2(hl, ax, ay);
    float Gg = anisotropicSmithGGX(wil, ax, ay);

    float absDotNV = std::fabs(glm::dot(record.normal, wi));

    Vec3 wol;
    if (randf(0.0f, 1.0f) < Fg) {
        reflect:
        s.pdf = (Fg * Dg * Gg) / (4.0f * absDotNV);
        wol = glm::normalize(glm::reflect(-wil, hl));
    } else {
        wol = glm::refract(-wil, hl, eta);
        if (glm::length(wol) < 0.0001f) goto reflect;
        wol = glm::normalize(wol);
        float dotHL = glm::dot(hl, wol);
        float denom = dotHV + eta * dotHL;
        s.pdf = ((1.0f - Fg) * Dg * Gg * std::abs(dotHV * dotHL * eta * eta)) / (absDotNV * denom * denom);
    }

    s.wo = glm::transpose(worldToLocal) * wol;

    return s;
}

float Glass::pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    float eta = record.frontFace ? (1.0f / params.ior) : params.ior;

    float aspect = std::sqrt(1.0f - 0.9f * params.anisotropic);
    float roughness2 = params.roughness * params.roughness;
    float ax = std::max(0.0001f, roughness2 / aspect);
    float ay = std::max(0.0001f, roughness2 * aspect);

    float dotNGV = glm::dot(record.geometricNormal, wi);
    float dotNGL = glm::dot(record.geometricNormal, wo);
    bool reflect = (dotNGV * dotNGL > 0.0f);
    
    Vec3 h = (reflect) ? glm::normalize(wi + wo) : -glm::normalize(wi + eta * wo);

    Mat3 worldToLocal = worldToLocal3x3(record.normal);
    Vec3 wil = worldToLocal * wi;
    Vec3 hl = worldToLocal * h;

    float dotHV = glm::dot(h, wi);
    float dotHL = glm::dot(h, wo);

    float Fg = fresnel(std::abs(dotHV), eta);
    float Dg = anisotropicGTR2(hl, ax, ay);
    float Gg = anisotropicSmithGGX(wil, ax, ay);

    float absDotNL = std::fabs(glm::dot(record.normal, wo));
    float absDotNV = std::fabs(glm::dot(record.normal, wi));

    if (reflect) {
        return (Fg * Dg * Gg) / (4.0f * absDotNV * absDotNL);
    } else {
        float dotHL = glm::dot(h, wo);
        float denom = dotHV + eta * dotHL;
        return ((1.0f - Fg) * Dg * Gg * std::abs(dotHV * dotHL * eta * eta)) / (absDotNV * absDotNL * denom * denom);
    }
}