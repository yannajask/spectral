#include "Dielectric.h"
#include "../geometry/HitRecord.h"

Dielectric::Dielectric(const Vec3& albedo, float ri) : albedo(albedo), ri(ri) {}

bool Dielectric::scatter(const Ray &ray, const HitRecord& record, Vec3& attenuation, Ray& scattered) const {
    attenuation = albedo;
    float ratio = record.frontFace ? (1.0f / ri) : ri;

    Vec3 unitDir = glm::normalize(ray.dir);
    float cosTheta = std::fmin(glm::dot(-unitDir, record.normal), 1.0f);
    float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

    Vec3 direction;

    if (ratio * sinTheta > 1.0f || schlick(cosTheta, ratio) > randf()) {
        direction = reflect(unitDir, record.normal);
    } else {
        direction = refract(unitDir, record.normal, ratio);
    }

    scattered = Ray(record.p, direction);
    return true;
}

float Dielectric::schlick(float cos, float ri) {
    float r0 = (1 - ri) / (1 + ri);
    r0 *= r0;
    return r0 + (1 - r0) * std::pow((1 - cos), 5);
}