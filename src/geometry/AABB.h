#pragma once

#include "../utils.h"

class AABB {
    public:
        AABB(const Vec3& vmin, const Vec3& vmax);

        bool hit(const Ray& ray) const;
        
    private:
        Vec3 bounds[2];
};