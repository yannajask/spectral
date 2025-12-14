#pragma once

#include "Ray.hpp"
#include "math.hpp"

class Camera {
    public:
        unsigned int width;
        unsigned int height;
        float fov;

        Mat4x4 view;
        float scale;
        float aspect;

        Camera(unsigned int width, unsigned int height, float fov, const Mat4x4& view)
            : width(width), height(height), fov(fov), view(view) {
            scale = tan(radians(fov * 0.5f));
            aspect = (float)width / (float)height;
        }

        Ray getRay(float u, float v) {
            float ndc_x = (2.0f * (u + 0.5f) / (float)width - 1.0f) * scale;
            float ndc_y = (1.0f - 2.0f * (v + 0.5f) / (float)height) * scale * 1.0f / aspect;

            Vec3 orig = Vec3(0.0f, 0.0f, 0.0f);
            Vec3 dir = glm::normalize(Vec3(ndc_x, ndc_y, -1.0f));

            Ray ray;
            ray.orig = transformPointMatrix(view, orig);
            ray.dir = transformDirMatrix(view, dir);
            return ray;
        }
};
