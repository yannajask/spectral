#pragma once

#include "../utils.h"
#include "HitRecord.h"

class Object {
    public:
        virtual ~Object() = default;
        virtual bool hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const = 0;
};