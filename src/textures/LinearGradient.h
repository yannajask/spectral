#pragma once

#include "Texture.h"

class LinearGradient : public Texture {
    public:
        LinearGradient(const Vec3& colour1, const Vec3& colour2) : colour1(colour1), colour2(colour2) {}
        virtual Vec3 evaluate(float u, float v, const Vec3& p) const override { return glm::mix(colour1, colour2, v); }

    private:
        Vec3 colour1, colour2;
};