#pragma once

#include "math.hpp"
#include "Ray.hpp"
#include "Intersection.hpp"

class Object {
    public:
        virtual ~Object() = default;
        virtual bool intersect(const Ray& ray, float tmin, float tmax, Intersection& intersection) const = 0;
};

class Sphere : public Object {
    public:
        Vec3 center;
        float radius;

        Sphere(const Vec3& center, float radius) : center(center), radius(radius) {}

        virtual bool intersect(const Ray& ray, float tmin, float tmax, Intersection& intersection) const override {
            Vec3 oc = center - ray.orig;
            float a = glm::length2(ray.dir);
            float h = glm::dot(ray.dir, oc);
            float c = glm::length2(oc) - (radius * radius);
            float discriminant = h * h - a * c;

            if (discriminant < 0.0f) return false;

            float sqrtd = std::sqrt(discriminant);

            float root = (h - sqrtd) / a;
            if (root < tmin || root > tmax) {
                root = (h + sqrtd) / a;
                if (root < tmin || root > tmax) return false;
            }

            intersection.t = root;
            intersection.p = ray.at(root);
            Vec3 outwardNormal = (intersection.p - center) / radius;
            intersection.setFaceNormal(ray, outwardNormal);

            return true;
        }
};

class Triangle : public Object {
    public:
        Vec3 v0, v1, v2;

        Triangle(const Vec3& a, const Vec3& b, const Vec3& c) : v0(a), v1(b), v2(c) {}

        // moller-trumbore algorithm
        virtual bool intersect(const Ray& ray, float tmin, float tmax, Intersection& intersection) const override {
            Vec3 e1 = v1 - v0;
            Vec3 e2 = v2 - v0;
            Vec3 pvec = glm::cross(ray.dir, e2);
            float determinant = glm::dot(e1, pvec);

            if (fabs(determinant) < 1e-6f) return false;

            float invDeterminant = 1.0f / determinant;

            Vec3 tvec = ray.orig - v0;
            float u = glm::dot(tvec, pvec) * invDeterminant;
            if (u < 0 || u > 1) return false;

            Vec3 qvec = glm::cross(tvec, e1);
            float v = glm::dot(ray.dir, qvec) * invDeterminant;
            if (v < 0 || u + v > 1) return false;

            float t = glm::dot(e2, qvec) * invDeterminant;
            if (t < tmin || t > tmax) return false;

            intersection.t = t;
            intersection.u = u;
            intersection.v = v;
            intersection.p = ray.at(t);
            Vec3 outwardNormal = glm::normalize(glm::cross(e1, e2));
            intersection.setFaceNormal(ray, outwardNormal);

            return true;
        }
};