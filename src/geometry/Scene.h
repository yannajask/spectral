#pragma once

#include "Object.h"
#include "Mesh.h"
#include "BVH.h"

class Scene : public Object {
    public:
        Scene() = default;

        void buildBVH();

        void add(shared_ptr<Object> object);
        void clear();

        virtual bool hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const override;

        virtual AABB bbox() const override;

    private:
        std::vector<shared_ptr<Object>> objects;
        unique_ptr<BVH> bvh;
};