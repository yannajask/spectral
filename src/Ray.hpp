#pragma once

#include "utils.hpp"

class Ray {
    public:
        Vec3 orig;
        Vec3 dir;

        Ray() {}

        Ray(const Vec3& orig, const Vec3& dir) : orig(orig), dir(dir) {}

        Vec3 at(float t) const {
            return orig + t * dir;
        }
};
