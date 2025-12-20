#pragma once

#include "../utils.h"

class AABB {
    public:
        AABB();
        AABB(const Vec3& vmin, const Vec3& vmax);
        AABB(const AABB& box1, const AABB& box2);

        bool hit(const Ray& ray, float tmin, float tmax) const;

        void add(const AABB& other);

        constexpr Vec3 min() const;
        constexpr Vec3 max() const;
        constexpr Vec3 center() const;

        constexpr unsigned int longestAxis() const;
        
    private:
        Vec3 bounds[2];
};

constexpr Vec3 AABB::min() const {
    return bounds[0];
}

constexpr Vec3 AABB::max() const {
    return bounds[1];
}

constexpr Vec3 AABB::center() const {
    return (bounds[0] + bounds[1]) * 0.5f;
}

constexpr unsigned int AABB::longestAxis() const {
    const Vec3 d = bounds[1] - bounds[0];
    return (d.x > d.y && d.x > d.z) ? 0 : (d.y > d.z ? 1 : 2);
}