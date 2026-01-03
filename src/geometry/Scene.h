#pragma once

#include "Object.h"
#include "BVH.h"
#include "../lights/Light.h"
#include "../lights/LightDistribution.h"

class Scene : public Object {
    public:
        Scene() = default;

        void buildBVH();
        void buildLights();

        void addObject(shared_ptr<Object> object);
        void addLight(shared_ptr<Light> light);

        virtual bool hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const override;

        virtual AABB bbox() const override;

        LightSample sampleLight(const HitRecord& record) const;

    private:
        std::vector<shared_ptr<Object>> objects;
        std::vector<shared_ptr<Light>> lights;

        unique_ptr<BVH> bvh;
        unique_ptr<LightDistribution> ld;
};