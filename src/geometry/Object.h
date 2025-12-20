#pragma once

#include "../utils.h"
#include "HitRecord.h"
#include "AABB.h"

class Object {
    public:
        virtual ~Object() = default;
        virtual bool hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const = 0;
        virtual AABB bbox() const = 0;
    
    protected:
        AABB _bbox;
};
