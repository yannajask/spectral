#pragma once

#include "../utils.h"

class Texture {
    public:
        virtual ~Texture() = default;
        virtual Vec3 evaluate(float u, float v, const Vec3& p) const = 0;
};