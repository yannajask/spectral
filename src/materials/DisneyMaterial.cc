#include "DisneyMaterial.h"

/*
#pragma once

#include "../utils.h"
#include "Material.h"

class DisneyMaterial : public Material {
    public:
        
        DisneyMaterial(Vec3 kd, float ns, float metallic = 0.0f) : baseColour(kd), roughness(ns), metallic(metallic) {

        }
        

        virtual bool scatter(const Ray& ray, const HitRecord& record, Vec3& attenuation, Ray& scattered) const {
            Vec3 wo = glm::normalize(-ray.dir);
            Vec3 wi = glm::normalize(scattered.dir);
            Vec3 normal = record.normal;

            float wDiffuse = (1.0f - metallic) * (1.0f - specTrans);
            float wSheen = (1.0f - metallic) * sheen;
            float wMetal = 1.0f - (specTrans * (1.0f - metallic));
            float wGlass = (1.0f - metallic) * specTrans;
            float wClearcoat = 0.25f * clearcoat;

            // if inside, set all weights to 0 and only leave glass lobes
            if (!record.frontFace) wDiffuse = wSheen = wMetal = wClearcoat = 0.0f;

            float dotNV = glm::dot(wo, normal);
            float dotNL = glm::dot(wi, normal);
            bool reflect = (dotNV * dotNL > 0.0f);

            Vec3 fd(0.0f);

            if (reflect) {
                if (wDiffuse > 0.0f) fd += wDiffuse * evaluateDiffuse();
                if (wMetal > 0.0f) fd += wMetal * evaluateMetal();
                if (wClearcoat > 0.0f) fd += wClearcoat * evaluateClearcoat();
                if (sheen > 0.0f) fd += wSheen * evaluateSheen();
                if (wGlass > 0.0f) fd += wGlass * evaluateGlassReflection();
            } else if (wGlass > 0.0f) {
                fd += wGlass * evaluateGlassTransmission();
            }

            return fd;
        }

        float scatterPdf(const Ray& ray, const HitRecord& record, const Ray& scattered) const {
        }

    private:
        Vec3 baseColour;
        float subsurface;
        float metallic;
        float specular;
        float specularTint;
        float roughness;
        float sheen;
        float sheenTint;
        float clearcoat;
        float clearcoatGloss;

        float ior;
        float specTrans;
        Vec3 scatterDistance;

        float diffTrans;
        float flatness;
        bool thin = false;

        inline Vec3 evaluateDiffuse(const Vec3& wo, const Vec3& h, const Vec3& wi, const Vec3& n) const {
            float absDotNL = absCosTheta(wi, n);
            float absDotNV = absCosTheta(wo, n);
            float dotHL = glm::dot(wi, h);

            float FL = std::pow((1.0 - absDotNL), 5);
            float FV = std::pow((1.0 - absDotNV), 5);
            float RR = 2.0f * roughness * (dotHL * dotHL);

            Vec3 fLambert = baseColour / pi;
            Vec3 fRetro = (baseColour / pi) * RR * (FL + FV + (FL * FV) * (RR - 1.0f));

            if (thin && flatness > 0.0f) {
                float fss90 = dotHL * dotHL * roughness * roughness;
                float fss = glm::mix(1.0f, fss90, FL) * glm::mix(1.0f, fss90, FV);
                float ss = 1.25f * (fss * (1.0f / (absDotNL + absDotNV) - 0.5f) + 0.5f);
                fLambert = glm::mix(fLambert, fLambert * ss * scatterDistance, flatness);
            }

            return fLambert * (1.0f - 0.5f * FL) * (1.0f - 0.5f * FV) + fRetro;
        }

        inline Vec3 calculateTint() const {
            float luminance = glm::dot(Vec3(0.3f, 0.6f, 1.0f), baseColour);
            return (luminance > 0.0f) ? baseColour * (1.0f / luminance) : Vec3(1.0f);
        }

        inline Vec3 evaluateSheen(const Vec3& wo, const Vec3& h, const Vec3& wi, const Vec3& n) const {
            if (sheen <= 0.0f) {
                return Vec3(0.0f);
            } else {
                float dotHL = glm::dot(h, wi);
                Vec3 tint = calculateTint();
                return sheen * glm::mix(Vec3(1.0f), tint, sheenTint) * schlick(dotHL);
            }
        }

        inline Vec3 evaluateTransmission(const Vec3& wi, const Vec3& wo, const Vec3& n) const {
            float absDotNL = absCosTheta(wi, n);
            float absDotNV = absCosTheta(wo, n);

            float eta = (glm::dot(wo, n) > 0.0f) ? (1.0f / ior) : ior;
            Vec3 ht = -glm::normalize(wi + eta * wo);

            float dotHL = glm::dot(ht, wi);
            float dotHV = glm::dot(ht, wo);

            float alpha = roughness * roughness;
            float G = seperableSmithGGXG1(wo, n, alpha) * seperableSmithGGXG1(wi, n, alpha);
            float D = GTR2(absCosTheta(ht, n), alpha);

            float r0 = (ior - 1.0f) / (ior + 1.0f);
            float F = schlick(dotHV, r0 * r0);

            Vec3 colour = thin ? glm::sqrt(baseColour) : baseColour;
            float c = (std::fabs(dotHL) * std::fabs(dotHV)) / (absDotNL * absDotNV);
            float t = (eta * eta) / ((dotHL + eta * dotHV) * (dotHL + eta * dotHV));

            return colour * c * t * (1.0f - F) * G * D;
        }

        inline Vec3 evaluateSpecular(const Vec3& wo, const Vec3& h, const Vec3& wi, const Vec3& n) const {
            float dotNL = glm::dot(wi, n);
            float dotNV = glm::dot(wo, n);
            if (dotNL <= 0.0f || dotNV < 0.0f) {
                return Vec3(0.0f);
            } else {
                float alpha = roughness * roughness;
                float G = seperableSmithGGXG1(wo, n, alpha) * seperableSmithGGXG1(wi, n, alpha);
                float D = GTR2(absCosTheta(h, n), alpha);

                float absDotVH = absCosTheta(wo, h);
                Vec3 tint = calculateTint();
                float r0 = (ior - 1.0f) / (ior + 1.0f);
                r0 *= r0;
                Vec3 F0 = r0 * specular * glm::mix(Vec3(1.0f), tint, specularTint);
                F0 = glm::mix(F0, baseColour, metallic);
                Vec3 F = schlick(absDotVH, F0);

                return (F * G * D) / (4.0f * std::abs(dotNL) * std::abs(dotNV));
            }
        }

        inline Vec3 evaluateClearcoat(const Vec3& wo, const Vec3& h, const Vec3& wi, const Vec3& n, float& fPDFw, float& rPDFw) const {
            if (clearcoat <= 0.0f) {
                return Vec3(0.0f);
            } else {
                float absDotNH = absCosTheta(h, n);
                float absDotNL = absCosTheta(wi, n);
                float absDotNV = absCosTheta(wo, n);
                float dotHL = glm::dot(h, wi);

                float D = GTR1(absDotNH, glm::mix(0.1f, 0.001f, clearcoatGloss));
                float F = schlick(dotHL);
                float G = seperableSmithGGXG1(wi, n, 0.25f) * seperableSmithGGXG1(wo, n, 0.25f);

                fPDFw = D / (4.0f * absDotNL);
                rPDFw = D / (4.0f * absDotNV);

                return Vec3(0.25f * clearcoat * F * G * D);
            }
        }

        static float schlick(float cosTheta, float F0 = 0.04f) {
            return F0 + (1.0f - F0) * std::pow((1.0f - cosTheta), 5);
        }

        static Vec3 schlick(float cosTheta, const Vec3& F0) {
            return F0 + ((Vec3(1.0f) - F0) * std::powf((1.0f - cosTheta), 5));
        }
};


*/

bool DisneyMaterial::scatter(const Ray& ray, const HitRecord& record, Vec3& attenuation, Ray& scattered) const {
    Vec3 wi = -glm::normalize(ray.dir);
    float dotNGL = glm::dot(record.geometricNormal, wi);

    float diffuseWeight   = 0.0f;
    float sheenWeight     = 0.0f;
    float metalWeight     = 0.0f;
    float glassWeight     = (1.0f - params.metallic) * params.specTrans; 
    float clearcoatWeight = 0.0f;

    if (dotNGL > 0.0f) {
        diffuseWeight   = (1.0f - params.metallic) * (1.0f - params.specTrans);
        sheenWeight     = (1.0f - params.metallic) * params.sheen;
        metalWeight     = (1.0f - params.specTrans * (1.0f - params.metallic));
        clearcoatWeight = 0.25f * params.clearcoat;
    }

    // ignore sheen for importance sampling
    float totalWeight = diffuseWeight + metalWeight + glassWeight + clearcoatWeight;

    diffuseWeight /= totalWeight;
    metalWeight /= totalWeight;
    glassWeight /= totalWeight;
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

    if (sheenWeight > 0.0f) attenuation += sheen.evaluate(wi, wo, record) * sheenWeight;

    scattered = Ray(record.p, wo);
    return true;
}

Vec3 Diffuse::evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    // fDiffuse = (1 - subsurface) * fBaseDiffuse + subsurface * fSubsurface
    Vec3 h = glm::normalize(wi + wo);
    float absDotNL = std::abs(glm::dot(record.normal, wi));
    float absDotNV = std::abs(glm::dot(record.normal, wo));
    float dotHV = glm::dot(h, wo);

    float FD90 = 0.5f + 2.0f * params.roughness * (dotHV * dotHV);
    float FDi = schlick(absDotNL, FD90);
    float FDo = schlick(absDotNV, FD90);

    Vec3 fBaseDiffuse = (params.baseColour / pi) * FDi * FDo;

    float Fss90 = params.roughness * (dotHV * dotHV);
    float Fssi = schlick(absDotNL, Fss90);
    float Fsso = schlick(absDotNV, Fss90);

    float lsLaw = (1.0f / (absDotNL + absDotNV)) - 0.5f;
    Vec3 fSubsurface = (1.25f * params.baseColour / pi) * (Fssi * Fsso * lsLaw + 0.5f);

    return glm::mix(fBaseDiffuse, fSubsurface, params.subsurface) * absDotNV;
}

Vec3 Metal::evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    // fMetal = (Fm * Dm * Gm) / (4 |n * wi|)
    float dotNL = glm::dot(record.normal, wi);
    float dotNV = glm::dot(record.normal, wo);

    if (dotNL <= 0.0f || dotNV <= 0.0f) return Vec3(0.0f);

    Mat3 worldToLocal = worldToLocal3x3(record.normal);
    Vec3 wil = worldToLocal * wi;
    Vec3 wol = worldToLocal * wo;

    Vec3 h = glm::normalize(wi + wo);
    Vec3 hl = glm::normalize(wil + wol);

    float absDotHV = std::abs(glm::dot(h, wo));
    float luminance = glm::dot(Vec3(0.3f, 0.6f, 1.0f), params.baseColour);
    Vec3 cTint = (luminance > 0.0f) ? params.baseColour / luminance : Vec3(1.0f);
    Vec3 Ks = (1.0f - params.specularTint) + params.specularTint * cTint;
    float R0 = ((params.ior - 1.0f) * (params.ior - 1.0f)) / ((params.ior + 1.0f) * (params.ior + 1.0f));
    Vec3 C0 = params.specular * R0 * (1.0f - params.metallic) * Ks + params.metallic * params.baseColour;
    Vec3 Fm = schlick(absDotHV, C0);

    float aspect = std::sqrt(1.0f - 0.9f * params.anisotropic);
    float roughness2 = params.roughness * params.roughness;
    float ax = std::max(0.0001f, roughness2 / aspect);
    float ay = std::max(0.0001f, roughness2 * aspect);
    float Dm = anisotropicGTR2(hl, ax, ay);

    float Gm = anisotropicSmithGGX(wil, ax, ay) * anisotropicSmithGGX(wol, ax, ay);

    return (Fm * Dm * Gm) / (4.0f * std::fabs(dotNL));
}

Vec3 Glass::evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    // fGlass = (baseColour * Fg * Dg * Gg) / (4 * |n * wi|) if reflect
    //        = (sqrt(baseColour)(1 - Fg) * Dg * Gg * |h * wo * h * wi|) / (|n * wi| * (h * wi + n * h * wi))
    Mat3 worldToLocal = worldToLocal3x3(record.normal);
    Vec3 wil = worldToLocal * wi;
    Vec3 wol = worldToLocal * wo;

    Vec3 h = glm::normalize(wi + wo);
    Vec3 hl = glm::normalize(wil + wol);

    float dotNGL = glm::dot(record.geometricNormal, wi);
    float dotNGV = glm::dot(record.geometricNormal, wo);
    float dotHL = glm::dot(h, wi);
    float dotHV = glm::dot(h, wo);

    float Rs = (dotHL - params.ior * dotHV) / (dotHL + params.ior * dotHV);
    float Rp = (params.ior * dotHL - dotHV) / (params.ior * dotHL + dotHV);
    float Fg = 0.5f * Rs * Rs + Rp * Rp;

    float aspect = std::sqrt(1.0f - 0.9f * params.anisotropic);
    float roughness2 = params.roughness * params.roughness;
    float ax = std::max(0.0001f, roughness2 / aspect);
    float ay = std::max(0.0001f, roughness2 * aspect);
    float Dg = anisotropicGTR2(hl, ax, ay);
        
    float Gg = anisotropicSmithGGX(wil, ax, ay) * anisotropicSmithGGX(wol, ax, ay);

    // this needs to be the geometric normal(hit record normal)
    float absDotNV = std::fabs(glm::dot(wo, record.normal));
    if (dotNGL * dotNGV > 0.0f) {
        float absDotNL = std::abs(glm::dot(record.normal, wi));
        return (params.baseColour * Fg * Dg * Gg) / (4.0f * absDotNL) * absDotNV;
    } else {
        float denom = dotHL + params.ior * dotHV;
        return (glm::sqrt(params.baseColour) * (1.0f - Fg) * Dg * Gg * std::abs(dotHL * dotHV)) / (std::abs(dotHL) * denom * denom) * absDotNV;
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

    float absDotHV = std::abs(glm::dot(h, wo));
    float Fc = schlick(absDotHV, 0.04f);

    float ag = (1.0f - params.clearcoatGloss) * 0.1f + params.clearcoatGloss * 0.001f;
    float Dc = GTR1(hl.z, ag);
    float Gc = smithGGX(wil, 0.25f) * smithGGX(wol, 0.25f);
    
    float absDotNL = std::abs(glm::dot(record.normal, wi));
    float absDotNV = std::abs(glm::dot(record.normal, wo));
    return Vec3((Fc * Dc * Gc) / (4.0f * std::fabs(absDotNL))) * absDotNV;
}

Vec3 Sheen::evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    // fSheen = cSheen (1 - |h * wo|)^5 *  |n * wo|
    if (params.sheen <= 0.0f) return Vec3(0.0f);

    Vec3 h = glm::normalize(wi + wo);
    float absDotNV = std::abs(glm::dot(record.normal, wo));
    float absDotHL = glm::dot(h, wi);
    float luminance = glm::dot(Vec3(0.3f, 0.6f, 1.0f), params.baseColour);
    Vec3 cTint = (luminance > 0.0f) ? params.baseColour / luminance : Vec3(1.0f);
    Vec3 cSheen = glm::mix(Vec3(1.0f), cTint, params.sheenTint);
    return cSheen * std::powf(1.0f - absDotHL, 5) * absDotNV;
}

// -----------------

// by default, lobes use cosine-weighted hemisphere sampling
Vec3 Lobe::sample(const Vec3& wo, const HitRecord& record, float& pdf, const Vec2& sampler) const {
    Vec3 wi = rand3fcd(record.normal);
    pdf = glm::dot(record.normal, wi) / pi;
    return wi;
}

Vec3 Metal::sample(const Vec3& wo, const HitRecord& record, float& pdf, const Vec2& sampler) const {
    float aspect = std::sqrt(1.0f - 0.9f * params.anisotropic);
    float roughness2 = params.roughness * params.roughness;
    float ax = std::max(0.0001f, roughness2 / aspect);
    float ay = std::max(0.0001f, roughness2 * aspect);

    Vec2 u = rand2f(0.0f, 1.0f);
    float phi = 2.0f * pi * u.x;
    float cosTheta2 = (1.0f - u.y) / (1.0f + (ax * ay - 1.0f) * u.x);
    float sinTheta = std::sqrtf(std::fmax(0.0f, 1.0f - cosTheta2));
    Vec3 hl = glm::normalize(Vec3(ax * sinTheta * std::cos(phi), ay * sinTheta * std::sin(phi), std::sqrtf(cosTheta2)));
    float Dm = anisotropicGTR2(hl, ax, ay);

    Mat3 worldToLocal = worldToLocal3x3(record.normal);
    Vec3 wol = worldToLocal * wo;
    Vec3 wil = glm::normalize(glm::reflect(-wol, hl));
    pdf = (Dm * hl.z) / (4.0f * std::fabs(glm::dot(hl, wol)));

    return glm::transpose(worldToLocal) * wil;
}


Vec3 Glass::sample(const Vec3& wo, const HitRecord& record, float& pdf, const Vec2& sampler) const {
    Vec2 u = rand2f(0.0f, 1.0f);
    // todo
}

Vec3 Clearcoat::sample(const Vec3& wo, const HitRecord& record, float& pdf, const Vec2& sampler) const {
    Vec2 u = rand2f(0.0f, 1.0f);
    // todo
}