#pragma once

#include "utils.h"
#include "geometry/Scene.h"
#include "geometry/HitRecord.h"
#define LODEPNG_COMPILE_CPP
#include "../extern/lodepng/lodepng.h"
#include <omp.h>

// to do: clean up initialization
// skybox feature could also be awesome

class Camera {
    public:
        unsigned int width, height;
        float fov, aspect, scale;

        unsigned int samplesPerPixel = 10;
        unsigned int maxDepth = 10;

        Vec3 background;

        Camera(const Vec3& lookfrom, const Vec3& lookat, unsigned int width, unsigned int height, float fov)
            : width(width), height(height), fov(fov), lookfrom(lookfrom) {
            scale = tan(radians(fov * 0.5f));
            aspect = (float)width / (float)height;

            w = glm::normalize(lookfrom - lookat);
            u = glm::normalize(glm::cross(up, w));
            v = glm::cross(w, u);
        }

        void render(const Scene& scene, std::string& outputPath) {
            std::vector<unsigned char> image(width * height * 4, 255);
            unsigned int rowsProcessed = 0;
            
            #pragma omp parallel for
            for (unsigned int row = 0; row < height; row++) {
                for (unsigned int col = 0; col < width; col++) {
                    Vec3 colour(0.0f);

                    for (unsigned int sample = 0; sample < samplesPerPixel; sample++) {
                        Ray ray = getRay(col, row);
                        colour += rayColour(ray, scene, maxDepth);
                    }

                    colour /= samplesPerPixel;

                    int r = int(255.999f * glm::clamp(colour.x, 0.0f, 1.0f));
                    int g = int(255.999f * glm::clamp(colour.y, 0.0f, 1.0f));
                    int b = int(255.999f * glm::clamp(colour.z, 0.0f, 1.0f));

                    size_t idx = 4 * (row * width + col);
                    image[idx + 0] = static_cast<unsigned char>(r);
                    image[idx + 1] = static_cast<unsigned char>(g);
                    image[idx + 2] = static_cast<unsigned char>(b);
                    image[idx + 3] = 255;
                }

                #pragma omp atomic
                rowsProcessed++;

                #pragma omp critical
                std::clog << "\rLine: " << rowsProcessed << " / " << height << "                " << std::flush;
            }

            lodepng::encode(outputPath, image, width, height);
            std::clog << "\rSaved to: " << outputPath << std::endl;
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

        Vec3 rayColour(const Ray& ray, const Scene& scene, unsigned int depth) const {
            if (depth <= 0) return Vec3(0.0f, 0.0f, 0.0f);

            HitRecord record;

            if (scene.hit(ray, 0.001, infinity, record)) {
                Ray scattered;
                Vec3 attenuation;

                if (record.mat->scatter(ray, record, attenuation, scattered)) {
                    return attenuation * rayColour(scattered, scene, depth - 1);
                }

                return Vec3(0.0f, 0.0f, 0.0f);
            } else {
                return background;
            }
        }

    private:
        Vec3 lookfrom;
        Vec3 u, v, w;
        inline static const Vec3 up{0.0f, 1.0f, 0.0f};
};