#pragma once

#include "Material.h"

class Dielectric : public Material {
    public:
        Dielectric(const Vec3& albedo, float ri);
        virtual bool scatter(const Ray &ray, const HitRecord& record, Vec3& attenuation, Ray& scattered) const override;

    private:
        Vec3 albedo;
        float ri;

        static float schlick(float cos, float ri);
};