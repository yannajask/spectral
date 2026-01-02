#include "Disney.h"
#include "../geometry/HitRecord.h"

DisneyBSDF::DisneyBSDF(const DisneyParams& p)
     : params(p), diffuse(params), metal(params), glass(params), clearcoat(params), sheen(params) {}

Vec3 DisneyBSDF::evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    LobeWeights w = calculateWeights(wi, record);

    Vec3 f(0.0f);

    if (w.diffuse > 0.0f)   f += w.diffuse * diffuse.evaluate(wi, wo, record);
    if (w.sheen > 0.0f)     f += w.sheen * sheen.evaluate(wi, wo, record);
    if (w.metal > 0.0f)     f += w.metal * metal.evaluate(wi, wo, record);
    if (w.glass > 0.0f)     f += w.glass * glass.evaluate(wi, wo, record);
    if (w.clearcoat > 0.0f) f += w.clearcoat * clearcoat.evaluate(wi, wo, record);

    return f;
}

BSDFSample DisneyBSDF::sample(const Vec3& wi, const HitRecord& record) const {
    BSDFSample s;

    LobeWeights w = calculateWeights(wi, record);
    float totalWeight = w.diffuse + w.metal + w.glass + w.clearcoat;
    float diffuseWeight = w.diffuse / totalWeight;
    float metalWeight   = w.metal / totalWeight;
    float glassWeight   = w.glass / totalWeight;

    float r = randf(0.0f, 1.0f);

    if (r < diffuseWeight) {
        s = diffuse.sample(wi, record);
    } else if (r < diffuseWeight + metalWeight) {
        s = metal.sample(wi, record);
    } else if (r < diffuseWeight + metalWeight + glassWeight) {
        s = glass.sample(wi, record);
    } else {
        s = clearcoat.sample(wi, record);
    }

    s.pdf = pdf(wi, s.wo, record);

    if (s.pdf > 0.0f) {
        Vec3 brdf = evaluate(wi, s.wo, record);
        float cosTheta = std::abs(glm::dot(record.normal, s.wo));
        s.f = brdf * cosTheta / s.pdf;
    } else {
        s.f = Vec3(0.0f);
    }

    return s;
}

float DisneyBSDF::pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    LobeWeights w = calculateWeights(wi, record);

    float totalWeight = w.diffuse + w.metal + w.glass + w.clearcoat;
    float totalPdf = 0.0f;

    if (w.diffuse > 0.0f)   totalPdf += w.diffuse * diffuse.pdf(wi, wo, record);
    if (w.metal > 0.0f)     totalPdf += w.metal * metal.pdf(wi, wo, record);
    if (w.glass > 0.0f)     totalPdf += w.glass * glass.pdf(wi, wo, record);
    if (w.clearcoat > 0.0f) totalPdf += w.clearcoat * clearcoat.pdf(wi, wo, record);

    return totalPdf / totalWeight;
}

DisneyBSDF::LobeWeights DisneyBSDF::calculateWeights(const Vec3& wi, const HitRecord& record) const {
    LobeWeights w;

    float dotNGV = glm::dot(record.geometricNormal, wi);

    if (dotNGV > 0.0f) {
        w.diffuse   = (1.0f - params.metallic) * (1.0f - params.specTrans);
        w.metal     = (1.0f - params.specTrans * (1.0f - params.metallic));
        w.clearcoat = 0.25f * params.clearcoat;
        w.sheen     = (1.0f - params.metallic) * params.sheen;
    }

    w.glass = (1.0f - params.metallic) * params.specTrans; 

    return w;
}


// ----------------

DisneyMaterial::DisneyMaterial(const DisneyParams& p) : params(p), bsdf(params) {}

Vec3 DisneyMaterial::evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    return bsdf.evaluate(wi, wo, record);
}

BSDFSample DisneyMaterial::sample(const Vec3& wi, const HitRecord& record) const {
    return bsdf.sample(wi, record);
}

float DisneyMaterial::pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const {
    return bsdf.pdf(wi, wo, record);
}