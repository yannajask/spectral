#pragma once

#include "utils.h"

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

        Ray getRay(float u, float v) const {
            Vec3 offset = sampleSquare();
            float px = u + offset.x;
            float py = v + offset.y;

            float ndc_x = (2.0f * (px + 0.5f) / (float)width - 1.0f) * scale * aspect;
            float ndc_y = (1.0f - 2.0f * (py + 0.5f) / (float)height) * scale;
            Vec3 orig = Vec3(0.0f, 0.0f, 0.0f);
            Vec3 dir = glm::normalize(Vec3(ndc_x, ndc_y, -1.0f));

            // to do: fix this later to make it prettier
            Ray ray;
            ray.orig = transformPointMatrix(view, orig);
            ray.dir = transformDirMatrix(view, dir);
            return Ray(ray.orig, ray.dir);
        }
};
