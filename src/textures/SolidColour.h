#pragma once

#include "Texture.h"

class SolidColour : public Texture {
    public:
        SolidColour(const Vec3& colour) : colour(colour) {}
        virtual Vec3 evaluate(float u, float v, const Vec3& p) const override { return colour; }

    private:
        Vec3 colour;
};