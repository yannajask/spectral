#pragma once

#include "Object.h"
#include "Triangle.h"

class Mesh : public Object {
    public:
        Mesh(const std::string& objPath) {

        }

        virtual bool hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const override {

        }

    private:
        std::vector<Triangle> triangles;
};