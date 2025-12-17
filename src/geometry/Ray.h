#pragma once

#include "../utils.h"

struct Ray {
    Vec3 orig;
    Vec3 dir;
    Vec3 invDir;
    int sign[3];

    Ray() {}

    Ray(const Vec3& orig, const Vec3& dir) : orig(orig), dir(dir) {
        invDir = 1.0f / dir;
        sign[0] = (invDir.x < 0);
        sign[1] = (invDir.y < 0);
        sign[2] = (invDir.z < 0);
    }

    Vec3 at(float t) const {
        return orig + t * dir;
    }
};
