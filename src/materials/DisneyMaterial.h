#pragma once

#include "../utils.h"
#include "Material.h"

struct MaterialParams {
    Vec3 baseColour;
    float subsurface;
    float metallic;
    float specular;
    float specularTint;
    float roughness;
    float anisotropic;
    float sheen;
    float sheenTint;
    float clearcoat;
    float clearcoatGloss;
    float ior;
    float specTrans;
};

struct Lobe {
    const MaterialParams& params;
    
    explicit Lobe(const MaterialParams& params) : params(params) {}
    virtual ~Lobe() = default;

    virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const = 0;
    virtual Vec3 sample(const Vec3& wo, const HitRecord& record, float& pdf, const Vec2& sampler) const;
    virtual float pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const = 0;
};

struct Diffuse : public Lobe {
    virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
    virtual float pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
};

struct Metal : public Lobe {
    virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
    virtual Vec3 sample(const Vec3& wo, const HitRecord& record, float& pdf, const Vec2& sampler) const override;
    virtual float pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
};

struct Glass : public Lobe {
    virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
    virtual Vec3 sample(const Vec3& wo, const HitRecord& record, float& pdf, const Vec2& sampler) const override;
    virtual float pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
};

struct Clearcoat : public Lobe {
    virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
    virtual Vec3 sample(const Vec3& wo, const HitRecord& record, float& pdf, const Vec2& sampler) const override;
    virtual float pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
};

struct Sheen : public Lobe {
    virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
    virtual float pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
};

class DisneyMaterial : public Material {
    public:
        explicit DisneyMaterial(const MaterialParams& params);

        virtual bool scatter(const Ray& ray, const HitRecord& record, Vec3& attenuation, Ray& scattered) const override;
        float pdf(const Ray& ray, const HitRecord& record, const Ray& scattered) const;

    private:
        MaterialParams params;

        // lobes
        Diffuse diffuse;
        Metal metal;
        Glass glass;
        Clearcoat clearcoat;
        Sheen sheen;
};
