#include <memory>
#include <iostream>

#include "geometry/Object.h"
#include "geometry/Triangle.h"
#include "geometry/Sphere.h"
#include "geometry/Scene.h"
#include "Camera.h"
#include "geometry/Ray.h"
#include "geometry/Mesh.h"
#include "utils.h"

// to do: move this elsewhere (and obvisouly make generic)
Vec3 rayColour(const Ray& ray, const Scene& scene, unsigned int depth) {
    if (depth <= 0) return Vec3(0, 0, 0);

    HitRecord record;
    record.t = infinity;
    if (scene.hit(ray, 0.001f, infinity, record)) {
        Vec3 direction = rand3fhs(record.normal);
        return 0.5f * rayColour(Ray(record.p, direction), scene, depth - 1);
    } else {
        Vec3 unitDirection = glm::normalize(ray.dir);
        float a = 0.5f * (unitDirection.y + 1.0f);
        return (1.0f - a) * Vec3(1.0f, 1.0f, 1.0f) + a * Vec3(0.5f, 0.7f, 1.0f);
    }
}

int main() {
    constexpr unsigned int samplesPerPixel = 10;
    constexpr unsigned int width = 800;
    constexpr unsigned int height = 600;
    
    Vec3 lookfrom = Vec3(0.0f, 5.0f, 40.0f);
    Vec3 lookat = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 up = Vec3(0.0f, 1.0f, 0.0f);

    Mat4x4 view = glm::lookAt(lookfrom, lookat, up);
    Camera camera(width, height, 75.0f, glm::inverse(view));

    Scene scene;
    scene.add(make_shared<Mesh>("assets/teddy.obj"));
    scene.buildBVH();

    std::cout << "P3\n" << width << " " << height << "\n255\n";
    for (unsigned int row = 0; row < height; row++) {
        std::clog << "\rLines: " << (row + 1) << " / " << height << "                 " << std::flush;

        for (unsigned int col = 0; col < width; col++) {
            Vec3 colour(0.0f);

            for (unsigned int sample = 0; sample < samplesPerPixel; sample++) {
                Ray ray = camera.getRay(col, row);
                colour += rayColour(ray, scene, 50);
            }

            colour /= samplesPerPixel;

            int r = int(255.999f * glm::clamp(colour.x, 0.0f, 1.0f));
            int g = int(255.999f * glm::clamp(colour.y, 0.0f, 1.0f));
            int b = int(255.999f * glm::clamp(colour.z, 0.0f, 1.0f));

            std::cout << r << ' ' << g << ' ' << b << '\n';
        }
    }
}