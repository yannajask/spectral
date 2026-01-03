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
#include "materials/Disney.h"
#include "textures/LinearGradient.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [output_name].png" << std::endl;
        return 1;
    }
    std::string file = argv[1];
    
    Vec3 lookfrom = Vec3(3.0f, 4.0f, 25.0f);
    Vec3 lookat = Vec3(0.0f, -1.0f, 0.0f);

    Camera camera(lookfrom, lookat, 800, 600, 75.0f);
    camera.samplesPerPixel = 500;
    camera.background = make_shared<LinearGradient>(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.02f, 0.05f, 0.2f));

    Scene scene;
    DisneyParams redParams = {
        Vec3(0.5f, 0.5f, 0.2f),
        0.0f,
        0.95f,
        0.0f,
        0.0f,
        0.3f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        1.5f,
        0.0f
    };

    auto redMat = make_shared<DisneyMaterial>(redParams);
    auto shuttle = make_shared<Mesh>("assets/cessna.obj", redMat);
    scene.addObject(make_shared<RotateY>(shuttle, 60.0f));
    scene.buildBVH();

    scene.addLight(make_shared<PointLight>(Vec3(1.0f, 0.4f, 0.4f), 20.0f, Vec3(0.0f, 10.0f, -4.0f)));
    scene.addLight(make_shared<DirectionalLight>(Vec3(0.6f, 0.7f, 1.0f), 2.0f, Vec3(0.0f, -1.0f, 0.0f)));
    scene.buildLights();

    auto start = std::chrono::high_resolution_clock::now();
    camera.render(scene, file);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time = end - start;
    std::clog << "\rTime: " << time.count() <<  "s              " << std::endl;
}