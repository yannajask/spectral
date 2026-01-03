#include "Scene.h"
#include <iostream>

void Scene::buildBVH() {
    bvh = make_unique<BVH>(objects);
}

void Scene::buildLights() {
    ld = make_unique<LightDistribution>(lights);
}

void Scene::addObject(shared_ptr<Object> object) {
    objects.push_back(object); 
}

void Scene::addLight(shared_ptr<Light> light) {
    lights.push_back(light); 
}

bool Scene::hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const {
    return bvh->hit(ray, tmin, tmax, record, bvh->root);
}

AABB Scene::bbox() const {
    return bvh->bbox();
}

LightSample Scene::sampleLight(const HitRecord& record) const {
    LightSample ls;
    if (!ld || lights.empty()) return ls;

    float pdf;
    float i = ld->sample(pdf);
    const auto& light = lights[i];
    ls = light->sample(record);
    ls.pdf *= pdf;

    return ls;
}