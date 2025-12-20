#include "Scene.h"
#include <iostream>

void Scene::buildBVH() {
    bvh = make_unique<BVH>(objects);
}

void Scene::add(shared_ptr<Object> object) {
    objects.push_back(object); 
}

void Scene::clear() {
    objects.clear();
}

bool Scene::hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const {
    return bvh->hit(ray, tmin, tmax, record, bvh->root);
}

/*
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
    */

AABB Scene::bbox() const {
    return bvh->bbox();
}