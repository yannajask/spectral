#include <memory>
#include <iostream>
#include <chrono>

#include "utils.h"
#include "Camera.h"
#include "geometry/Object.h"
#include "geometry/Transform.h"
#include "geometry/Triangle.h"
#include "geometry/Sphere.h"
#include "geometry/Scene.h"
#include "geometry/Ray.h"
#include "geometry/Mesh.h"
#include "materials/Material.h"
#include "materials/Dielectric.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [output_name].png" << std::endl;
        return 1;
    }
    std::string file = argv[1];
    
    Vec3 lookfrom = Vec3(2.0f, 10.0f, 10.0f);
    Vec3 lookat = Vec3(0.0f, 0.0f, 0.0f);

    Camera camera(lookfrom, lookat, 800, 600, 75.0f);
    camera.samplesPerPixel = 25;
    camera.maxDepth = 25;
    camera.background = Vec3(0.6f, 0.7f, 1.0f);

    Scene scene;
    auto redMat = make_shared<Dielectric>(Vec3(0.9f, 0.6f, 0.6f), 1.33f);
    auto shuttle = make_shared<Mesh>("assets/shuttle.obj", redMat);
    scene.add(make_shared<RotateY>(shuttle, 45.0f));
    scene.buildBVH();

    auto start = std::chrono::high_resolution_clock::now();
    camera.render(scene, file);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time = end - start;
    std::clog << "\rTime: " << time.count() <<  "s              " << std::endl;
}