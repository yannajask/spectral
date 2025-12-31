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
    
    explicit Lobe(const MaterialParams& p) : params(p) {}
    virtual ~Lobe() = default;

    virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const = 0;
    virtual Vec3 sample(const Vec3& wi, const HitRecord& record, float& pdf) const;
};

struct Diffuse : public Lobe {
    explicit Diffuse(const MaterialParams& p) : Lobe(p) {}
    virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
};

struct Metal : public Lobe {
    explicit Metal(const MaterialParams& p) : Lobe(p) {}
    virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
    virtual Vec3 sample(const Vec3& wi, const HitRecord& record, float& pdf) const override;
};

struct Glass : public Lobe {
    explicit Glass(const MaterialParams& p) : Lobe(p) {}
    virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
    virtual Vec3 sample(const Vec3& wi, const HitRecord& record, float& pdf) const override;
};

struct Clearcoat : public Lobe {
    explicit Clearcoat(const MaterialParams& p) : Lobe(p) {}
    virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
    virtual Vec3 sample(const Vec3& wi, const HitRecord& record, float& pdf) const override;
};

struct Sheen : public Lobe {
    explicit Sheen(const MaterialParams& p) : Lobe(p) {}
    virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
};

class DisneyMaterial : public Material {
    public:
        explicit DisneyMaterial(const MaterialParams& p);
        virtual bool scatter(const Ray& ray, const HitRecord& record, Vec3& attenuation, Ray& scattered) const override;

    private:
        MaterialParams params;

        // lobes
        Diffuse diffuse;
        Metal metal;
        Glass glass;
        Clearcoat clearcoat;
        Sheen sheen;
};
