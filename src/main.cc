#include <memory>
#include <iostream>
#include <chrono>

#include "geometry/Object.h"
#include "geometry/Triangle.h"
#include "geometry/Sphere.h"
#include "geometry/Scene.h"
#include "Camera.h"
#include "geometry/Ray.h"
#include "geometry/Mesh.h"
#include "utils.h"
#define LODEPNG_COMPILE_CPP
#include "../extern/lodepng/lodepng.h"

// to do: move this elsewhere (and obvisouly make generic)
Vec3 rayColour(const Ray& ray, const Scene& scene, unsigned int depth) {
    if (depth <= 0) return Vec3(0, 0, 0);

    HitRecord record;
    record.t = infinity;
    if (scene.hit(ray, 0.001f, infinity, record)) {
        //Vec3 direction = rand3fhs(record.normal);
        Vec3 normalColour = 0.5f * (record.normal + Vec3(1.0f, 1.0f, 1.0f));
        return normalColour;
        //return 0.5f * rayColour(Ray(record.p, direction), scene, depth - 1);
    } else {
        Vec3 unitDirection = glm::normalize(ray.dir);
        float a = 0.5f * (unitDirection.y + 1.0f);
        return (1.0f - a) * Vec3(1.0f, 1.0f, 1.0f) + a * Vec3(0.5f, 0.7f, 1.0f);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [output_name].png" << std::endl;
        return 1;
    }
    std::string file = argv[1];

    constexpr unsigned int samplesPerPixel = 10;
    constexpr unsigned int width = 800;
    constexpr unsigned int height = 600;
    
    Vec3 lookfrom = Vec3(0.0f, 5.0f, 40.0f);
    Vec3 lookat = Vec3(0.0f, 0.0f, 0.0f);

    Camera camera(lookfrom, lookat, width, height, 75.0f);

    Scene scene;
    scene.add(make_shared<Mesh>("assets/teddy.obj"));
    scene.buildBVH();

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<unsigned char> image(width * height * 4, 255);    
    for (unsigned int row = 0; row < height; row++) {
        std::clog << "\rLine: " << (row + 1) << " / " << height << "                " << std::flush;

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

            size_t idx = 4 * (row * width + col);
            image[idx + 0] = static_cast<unsigned char>(r);
            image[idx + 1] = static_cast<unsigned char>(g);
            image[idx + 2] = static_cast<unsigned char>(b);
            image[idx + 3] = 255;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time = end - start;
    std::clog << "\rTime: " << time.count() <<  "s              " << std::endl;

    lodepng::encode(file, image, width, height);
    std::clog << "Saved to: " << file << std::endl;
}