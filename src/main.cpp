#include <memory>
#include <iostream>

#include "Object.hpp"
#include "Scene.hpp"
#include "Camera.hpp"
#include "Ray.hpp"

// to do: move this elsewhere (and obvisouly make generic)
Vec3 rayColour(const Ray& ray, const Scene& scene) {
    Intersection intersection;
    if (scene.intersect(ray, 0.001f, infinity, intersection)) {
        return 0.5f * (intersection.normal + Vec3(1.0f));
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
    Camera camera(width, height, 45.0f, Mat4x4(1.0f));

    Scene scene;
    scene.add(std::make_shared<Sphere>(Vec3(0.0f, 0.0f, -5.0f), 1.0f));
    scene.add(std::make_shared<Triangle>(
        Vec3(-1.0f, -1.0f, -5.0f),
        Vec3( 1.0f, -1.0f, -5.0f),
        Vec3( 0.0f,  1.0f, -5.0f)
    ));

    std::cout << "P3\n" << width << " " << height << "\n255\n";
    for (unsigned int row = 0; row < height; row++) {
        for (unsigned int col = 0; col < width; col++) {
            Vec3 colour(0.0f);

            for (unsigned int sample = 0; sample < samplesPerPixel; sample++) {
                Ray ray = camera.getRay(col, row);
                colour += rayColour(ray, scene);
            }

            colour /= samplesPerPixel;

            int r = int(255.999f * glm::clamp(colour.x, 0.0f, 1.0f));
            int g = int(255.999f * glm::clamp(colour.y, 0.0f, 1.0f));
            int b = int(255.999f * glm::clamp(colour.z, 0.0f, 1.0f));

            std::cout << r << ' ' << g << ' ' << b << '\n';
        }
    }
}