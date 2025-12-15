#include "Scene.h"

bool Scene::hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const {
    HitRecord tmp;
    bool hit = false;
    float closest = tmax;

    for (const auto& object: objects) {
        if (object->hit(ray, tmin, closest, tmp)) {
            hit = true;
            closest = tmp.t;
            record = tmp;
        }
    }

    return hit;
}