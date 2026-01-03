#pragma once

#include "utils.h"
#include "geometry/Scene.h"
#include "geometry/HitRecord.h"
#include "materials/Material.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../extern/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../extern/stb/stb_image_write.h"
#include "omp.h"

// to do: clean up initialization
// skybox feature could also be awesome

class Camera {
    public:
        unsigned int width, height;
        float fov, aspect, scale;

        unsigned int samplesPerPixel = 10;
        unsigned int minDepth = 3;
        unsigned int maxDepth = 15;

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
            std::vector<unsigned char> image(width * height * 3, 255);
            unsigned int rowsProcessed = 0;
            
            #pragma omp parallel for
            for (unsigned int row = 0; row < height; row++) {
                for (unsigned int col = 0; col < width; col++) {
                    Vec3 colour(0.0f);

                    for (unsigned int sample = 0; sample < samplesPerPixel; sample++) {
                        Ray ray = getRay(col, row);
                        colour += rayColour(ray, scene);
                    }

                    colour /= samplesPerPixel;
                    colour = glm::sqrt(colour);

                    int r = int(255.999f * glm::clamp(colour.x, 0.0f, 1.0f));
                    int g = int(255.999f * glm::clamp(colour.y, 0.0f, 1.0f));
                    int b = int(255.999f * glm::clamp(colour.z, 0.0f, 1.0f));

                    size_t idx = 3 * (row * width + col);
                    image[idx + 0] = static_cast<unsigned char>(r);
                    image[idx + 1] = static_cast<unsigned char>(g);
                    image[idx + 2] = static_cast<unsigned char>(b);
                }

                #pragma omp atomic
                rowsProcessed++;

                #pragma omp critical
                std::clog << "\rLine: " << rowsProcessed << " / " << height << "                " << std::flush;
            }

            stbi_write_png(outputPath.c_str(), width, height, 3, image.data(), width * 3);
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

        Vec3 rayColour(const Ray& ray, const Scene& scene) const {
            Ray current = ray;
            Vec3 throughput(1.0f);
            Vec3 radiance(0.0f);

            for (unsigned int depth = 0; depth < maxDepth; depth++) {
                HitRecord record;

                if (!scene.hit(current, 0.001f, infinity, record)) {
                    radiance += throughput * background;
                    break;
                }

                Vec3 wi = -current.dir;

                LightSample ls = scene.sampleLight(record);
                Vec3 f = record.mat->evaluate(wi, ls.wo, record);
                float cosTheta = std::max(0.0f, glm::dot(record.normal, ls.wo));

                HitRecord tmp;
                if (!scene.hit(Ray(record.p, ls.wo), 0.001f, ls.dist - 0.001f, tmp)) {
                    radiance += (throughput * f * ls.Li * cosTheta) / ls.pdf;
                }

                BSDFSample s = record.mat->sample(wi, record);
                if (s.pdf <= 0.0f) break;

                Vec3 fNext = record.mat->evaluate(wi, s.wo, record);
                float cosThetaNext = std::abs(glm::dot(record.normal, s.wo));
                throughput *= (fNext * cosThetaNext) / s.pdf;

                current = Ray(record.p, s.wo);

                if (depth >= minDepth) {
                    float p = std::max({throughput.r, throughput.g, throughput.b});
                    p = std::clamp(p, 0.05f, 0.95f);
                    if (randf() > p) break;
                    throughput /= p;
                }
            }

            return radiance;
        }

    private:
        Vec3 lookfrom;
        Vec3 u, v, w;
        inline static const Vec3 up{0.0f, 1.0f, 0.0f};
};