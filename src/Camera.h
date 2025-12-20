#pragma once

#include "utils.h"

class Camera {
    public:
        unsigned int width, height;
        float fov, aspect, scale;

        Camera(const Vec3& lookfrom, const Vec3& lookat, unsigned int width, unsigned int height, float fov)
            : lookfrom(lookfrom), width(width), height(height), fov(fov) {
            scale = tan(radians(fov * 0.5f));
            aspect = (float)width / (float)height;

            w = glm::normalize(lookfrom - lookat);
            u = glm::normalize(glm::cross(up, w));
            v = glm::cross(w, u);
        }

        Ray getRay(float i, float j) const {
            Vec3 offset = sampleSquare();

            float x = (i + 0.5f + offset.x) / (float)width;
            float y = (j + 0.5f + offset.y) / (float)height;

            float px = (2.0f * x - 1.0f) * scale * aspect;
            float py = (1.0f - 2.0f * y) * scale;

            Vec3 dir = glm::normalize(px * u + py * v - w);
            return Ray(lookfrom, dir);
        }

    private:
        Vec3 lookfrom;
        Vec3 u, v, w;
        inline static const Vec3 up{0.0f, 1.0f, 0.0f};

};
