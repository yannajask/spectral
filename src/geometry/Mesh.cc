#include "Mesh.h"

explicit Mesh::Mesh(const std::string& objPath) {
    // to do: move this into a utility class or function
    // also want to expand functionality later
    std::ifstream objFile(objPath);
    std::vector<Vec3> vertices;
    std::string line;

    while (std::getline(objFile, line)) {
        std::stringstream ss(line);
        std::string op;
        ss >> op;

        if (op == "v") {
            Vec3 v;
            ss >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        } else if (op == "f") {
            std::string v0, v1, v2;
            ss >> v0 >> v1 >> v2;
            int i = 0, j = 0, k = 0;

            std::from_chars(v0.data(), v0.data() + v0.size(), i);
            std::from_chars(v1.data(), v1.data() + v1.size(), j);
            std::from_chars(v2.data(), v2.data() + v2.size(), k);

            addTriangle(vertices[i - 1], vertices[j - 1], vertices[k - 1]);
        }
    }
}

void Mesh::addTriangle(const Triangle& triangle) {
    triangles.push_back(triangle);
}

void Mesh::addTriangle(const Vec3& a, const Vec3& b, const Vec3&c) {
    triangles.push_back(Triangle(a, b, c));
}

bool Mesh::hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const {
    HitRecord tmp;
    bool hit = false;
    float closest = tmax;

    for (const Triangle& triangle: triangles) {
        if (triangle.hit(ray, tmin, closest, tmp)) {
            hit = true;
            closest = tmp.t;
            record = tmp;
        }
    }

    return hit;
}