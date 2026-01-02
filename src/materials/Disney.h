#pragma once

#include "../utils.h"
#include "BSDF.h"

struct DisneyParams {
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

class Diffuse : public BSDF {
    public:
        explicit Diffuse(const DisneyParams& p) : params(p) {}
        virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
        virtual BSDFSample sample(const Vec3& wi, const HitRecord& record) const override;
        virtual float pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;

    private:
        const DisneyParams& params;
};

class Metal : public BSDF {
    public:
        explicit Metal(const DisneyParams& p) : params(p) {}
        virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
        virtual BSDFSample sample(const Vec3& wi, const HitRecord& record) const override;
        virtual float pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;

    private:
        const DisneyParams& params;
};

class Glass : public BSDF {
    public:
        explicit Glass(const DisneyParams& p) : params(p) {}
        virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
        virtual BSDFSample sample(const Vec3& wi, const HitRecord& record) const override;
        virtual float pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;

    private:
        const DisneyParams& params;
};

class Clearcoat : public BSDF {
    public:
        explicit Clearcoat(const DisneyParams& p) : params(p) {}
        virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
        virtual BSDFSample sample(const Vec3& wi, const HitRecord& record) const override;
        virtual float pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;

    private:
        const DisneyParams& params;
};

class Sheen : public BSDF {
    public:
        explicit Sheen(const DisneyParams& p) : params(p) {}
        virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
        virtual BSDFSample sample(const Vec3& wi, const HitRecord& record) const override;
        virtual float pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;

    private:
        const DisneyParams& params;
};

class DisneyBSDF : public BSDF {
    public:
        explicit DisneyBSDF(const DisneyParams& p);
        virtual Vec3 evaluate(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;
        virtual BSDFSample sample(const Vec3& wi, const HitRecord& record) const override;
        virtual float pdf(const Vec3& wi, const Vec3& wo, const HitRecord& record) const override;

    private:
        DisneyParams params;

        // lobes
        Diffuse diffuse;
        Metal metal;
        Glass glass;
        Clearcoat clearcoat;
        Sheen sheen;

        // weights
        struct LobeWeights {
            float diffuse   = 0.0f;
            float metal     = 0.0f;
            float glass     = 0.0f;
            float clearcoat = 0.0f;
            float sheen     = 0.0f;
        };

        LobeWeights calculateWeights(const Vec3& wi, const HitRecord& record) const;
};
