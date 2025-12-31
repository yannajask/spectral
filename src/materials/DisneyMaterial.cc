#include "DisneyMaterial.h"

DisneyMaterial::DisneyMaterial(const MaterialParams& p)
     : params(p), diffuse(params), metal(params), glass(params), clearcoat(params), sheen(params) {}

bool DisneyMaterial::scatter(const Ray& ray, const HitRecord& record, Vec3& attenuation, Ray& scattered) const {
    Vec3 wi = -glm::normalize(ray.dir);
    float dotNGV = glm::dot(record.geometricNormal, wi);

    float diffuseWeight   = 0.0f;
    float sheenWeight     = 0.0f;
    float metalWeight     = 0.0f;
    float glassWeight     = (1.0f - params.metallic) * params.specTrans; 
    float clearcoatWeight = 0.0f;

    if (dotNGV > 0.0f) {
        diffuseWeight   = (1.0f - params.metallic) * (1.0f - params.specTrans);
        sheenWeight     = (1.0f - params.metallic) * params.sheen;
        metalWeight     = (1.0f - params.specTrans * (1.0f - params.metallic));
        clearcoatWeight = 0.25f * params.clearcoat;
    }

    // ignore sheen for importance sampling
    float totalWeight = diffuseWeight + metalWeight + glassWeight + clearcoatWeight;

    diffuseWeight   /= totalWeight;
    metalWeight     /= totalWeight;
    glassWeight     /= totalWeight;
    clearcoatWeight /= totalWeight;

    float r = randf(0.0f, 1.0f);
    float pdf = 0.0f;
    Vec3 wo;

    if (r < diffuseWeight) {
        wo = diffuse.sample(wi, record, pdf);
        pdf *= diffuseWeight;
        attenuation = diffuse.evaluate(wi, wo, record);
    } else if (r < diffuseWeight + metalWeight) {
        wo = metal.sample(wi, record, pdf);
        pdf *= metalWeight;
        attenuation = metal.evaluate(wi, wo, record);
    } else if (r < diffuseWeight + metalWeight + glassWeight) {
        wo = glass.sample(wi, record, pdf);
        pdf *= glassWeight;
        attenuation = glass.evaluate(wi, wo, record);
    } else {
        wo = clearcoat.sample(wi, record, pdf);
        pdf *= clearcoatWeight;
        attenuation = clearcoat.evaluate(wi, wo, record);
    }

    if (sheenWeight > 0.0f) {
        attenuation += sheen.evaluate(wi, wo, record) * sheenWeight;
    }

    if (pdf > 0.0f) {
        float cosTheta = std::abs(glm::dot(record.normal, wo));
        attenuation *= cosTheta / pdf;
    } else {
        attenuation = Vec3(0.0f);
    }

    scattered = Ray(record.p, wo);
    return true;
}

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

    return (Fm * Dm * Gm) / (4.0f * std::fabs(dotNV));
}

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
    return Vec3((Fc * Dc * Gc) / (4.0f * std::fabs(absDotNV)));
}

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

// -----------------

// by default, lobes use cosine-weighted hemisphere sampling
Vec3 Lobe::sample(const Vec3& wi, const HitRecord& record, float& pdf) const {
    Vec3 wo = rand3fcd(record.normal);
    pdf = glm::dot(record.normal, wo) / pi;
    return wo;
}

Vec3 Metal::sample(const Vec3& wi, const HitRecord& record, float& pdf) const {
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
    float s = 0.5f * (1.0f + vh.z);
    t2 = (1.0f - s) * std::sqrt(1.0f - t1 * t1) + s * t2;

    Vec3 nh = t1 * T1 + t2 * T2 + std::sqrt(std::fmax(0.0f, 1.0f - t1 * t1 - t2 * t2)) * vh;
    Vec3 hl = glm::normalize(Vec3(ax * nh.x, ay * nh.y, std::fmax(0.0f, nh.z)));

    float Dm = anisotropicGTR2(hl, ax, ay);
    float Gwi = anisotropicSmithGGX(wil, ax, ay);

    pdf = (Dm * Gwi) / (4.0f * std::fabs(glm::dot(record.normal, wi)));

    Vec3 wol = glm::normalize(glm::reflect(-wil, hl));
    return glm::transpose(worldToLocal) * wol;
}


Vec3 Glass::sample(const Vec3& wi, const HitRecord& record, float& pdf) const {
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
    float s = 0.5f * (1.0f + vh.z);
    t2 = (1.0f - s) * std::sqrt(1.0f - t1 * t1) + s * t2;

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
        pdf = (Fg * Dg * Gg) / (4.0f * absDotNV);
        wol = glm::normalize(glm::reflect(-wil, hl));
    } else {
        wol = glm::refract(-wil, hl, eta);
        if (glm::length(wol) < 0.0001f) goto reflect;
        wol = glm::normalize(wol);
        float dotHL = glm::dot(hl, wol);
        float denom = dotHV + eta * dotHL;
        pdf = ((1.0f - Fg) * Dg * Gg * std::abs(dotHV * dotHL * eta * eta)) / (absDotNV * denom * denom);
    }

    return glm::transpose(worldToLocal) * wol;
}

Vec3 Clearcoat::sample(const Vec3& wi, const HitRecord& record, float& pdf) const {
    float ag = (1.0f - params.clearcoatGloss) * 0.1f + params.clearcoatGloss * 0.001f;

    Vec2 u = rand2f(0.0f, 1.0f);
    float cosTheta = std::sqrtf((1.0f - std::powf(ag * ag, 1.0f - u.x)) / (1.0f - ag * ag));
    float sinTheta = std::sqrtf(1.0f - cosTheta * cosTheta);
    float phi = 2.0f * pi * u.y;
    Vec3 hl = Vec3(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);

    Mat3 localToWorld = glm::transpose(worldToLocal3x3(record.normal));
    Vec3 h = glm::normalize(localToWorld * hl);
    Vec3 wo = glm::normalize(glm::reflect(-wi, h));

    float Dc = GTR1(hl.z, ag);
    pdf = (Dc * std::fabs(glm::dot(record.normal, h))) / (4.0f * std::fabs(glm::dot(wo, h)));

    return wo;
}

/*
https://jcgt.org/published/0007/04/01/slides.pdf

Vec3 vh = normalize(Vec3(ax * Ve.x, ay * Ve.y, Ve.z));
// orthonormal basis
Vec3 T1 = (vh.z < 0.9999f) ? normalize(cross(Vec3(0.0f, 0.0f, 1.0f), vh)) : Vec3(1.0f, 0.0f, 0.0f);
Vec3 T2 = cross(vh, T1);

float r = sqrt(u.x);
float phi = 2.0f * pi * u.y;
float t1 = r * cos(phi);
float t2 = r * sin(phi);
float s = 0.5f * (1.0f + vh.z);
t2 = (1.0f - s) * sqrt(1.0 - t1 * t1) + s * t2;
Vec3 nh = t1 * T1 + t2 * T2 + sqrt(max(0.0f, 1.0f - t1 * t1 - t2 * t2)) * vh;
Vec3 Ne = normalize(Vec3(ax * nh.x, ay * nh.y, std::max(0.0f, nh.z)));

*/