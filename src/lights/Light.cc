#include "Light.h"

LightSample DirectionalLight::sample(const HitRecord& record) const {
    LightSample ls;
    ls.Li = colour * luminance;
    ls.wo = -dir;
    ls.dist = infinity;
    ls.pdf = 1.0f;
    ls.isDelta = true;
    return ls;
}

float DirectionalLight::power() const { return luminance; }

LightSample PointLight::sample(const HitRecord& record) const {
    LightSample ls;

    Vec3 dir = position - record.p;
    float dist = glm::length(dir);

    ls.Li = (colour * luminance) / (4.0f * pi * dist * dist);
    ls.wo = dir / dist;
    ls.dist = dist;
    ls.isDelta = true;
    ls.pdf = 1.0f;
    return ls;
}

float PointLight::power() const { return luminance; }

LightSample InfiniteLight::sample(const HitRecord& record) const {
    LightSample ls;
    ls.Li = colour * luminance / pi;
    ls.wo = rand3fcd(record.normal);
    ls.dist = infinity;
    ls.pdf = glm::dot(record.normal, ls.wo) / pi;
    ls.isDelta = false;
    return ls;
}

float InfiniteLight::power() const { return luminance * 4.0f * pi; }