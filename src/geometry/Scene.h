#pragma once

#include "Object.h"

class Scene : public Object {
    public:
        void add(std::shared_ptr<Object> object) { objects.push_back(object); }
        void clear() { objects.clear(); }

        virtual bool hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const override;

    private:
        std::vector<std::shared_ptr<Object>> objects;
};