#pragma once

#include "Object.hpp"

class Scene : public Object {
    public:
        std::vector<std::shared_ptr<Object>> objects;

        void add(std::shared_ptr<Object> object) { objects.push_back(object); }
        void clear() { objects.clear(); }

        virtual bool intersect(const Ray& ray, float tmin, float tmax, Intersection& intersection) const override {
            Intersection tmp;
            bool hit = false;
            float closest = tmax;

            for (const auto& object: objects) {
                if (object->intersect(ray, tmin, closest, tmp)) {
                    hit = true;
                    closest = tmp.t;
                    intersection = tmp;
                }
            }

            return hit;
        }
};