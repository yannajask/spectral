#include "Mesh.h"
#include "../ObjLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <charconv>

Mesh::Mesh(const std::string& objPath, shared_ptr<Material> mat) : mat(mat) {
    // to do: move this into a utility class or function
    std::ifstream objFile(objPath);

    std::vector<Vec3> vertices;
    std::vector<Vec2> uvs;
    std::vector<Vec3> normals;

    std::string line;
    while (std::getline(objFile, line)) {
        std::string_view view(line);
        size_t split = view.find(' ');
        if (split == std::string_view::npos) continue;

        std::string_view type = view.substr(0, split);
        std::string data(view.substr(split + 1));
        std::stringstream ss(data);

        if (type == "v") {
            Vec3 v;
            ss >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        } else if (type == "vt") {
            Vec2 vt;
            ss >> vt.x >> vt.y;
            uvs.push_back(vt);
        } else if (type == "vn") {
            Vec3 vn;
            ss >> vn.x >> vn.y >> vn.z;
            normals.push_back(vn);
        } else if (type == "f") {
            std::vector<FaceVertex> faceVertices;
            std::string_view data = view.substr(split + 1);
            //FaceVertex fv[3];
            size_t start = 0;
            while (true) {
                start = data.find_first_not_of(' ', start);
                if (start == std::string_view::npos) break;

                size_t end = data.find(' ', start);
                std::string_view tuple = data.substr(start, end - start);
 
                FaceVertex fv;
                parseFaceVertex(tuple, fv);
                faceVertices.push_back(fv);

                start = end;
            }

            for (size_t i = 1; i < faceVertices.size() - 1; i++) {
                const FaceVertex& fv0 = faceVertices[0];
                const FaceVertex& fv1 = faceVertices[i];
                const FaceVertex& fv2 = faceVertices[i + 1];

                Vec3 v0 = vertices[fv0.vertexIdx];
                Vec3 v1 = vertices[fv1.vertexIdx];
                Vec3 v2 = vertices[fv2.vertexIdx];

                Vec2 t0 = (fv0.uvIdx != -1) ? uvs[fv0.uvIdx] : Vec2(0.0f, 0.0f);
                Vec2 t1 = (fv1.uvIdx != -1) ? uvs[fv1.uvIdx] : Vec2(1.0f, 0.0f);
                Vec2 t2 = (fv2.uvIdx != -1) ? uvs[fv2.uvIdx] : Vec2(0.0f, 1.0f);

                Vec3 n0, n1, n2;
                if (fv0.normalIdx != -1) {
                    n0 = normals[fv0.normalIdx];
                    n1 = normals[fv1.normalIdx];
                    n2 = normals[fv2.normalIdx];
                } else {
                    Vec3 faceNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
                    n0 = n1 = n2 = faceNormal;
                }

                addTriangle(make_shared<Triangle>(v0, v1, v2, t0, t1, t2, n0, n1, n2, mat));
            }
            /*
                Vec3 v0 = vertices[fv[0].vertexIdx];
                Vec3 v1 = vertices[fv[1].vertexIdx];
                Vec3 v2 = vertices[fv[2].vertexIdx];

                Vec2 t0 = (fv[0].uvIdx != -1) ? uvs[fv[0].uvIdx] : Vec2(0.0f, 0.0f);
                Vec2 t1 = (fv[1].uvIdx != -1) ? uvs[fv[1].uvIdx] : Vec2(1.0f, 0.0f);
                Vec2 t2 = (fv[2].uvIdx != -1) ? uvs[fv[2].uvIdx] : Vec2(0.0f, 1.0f);

                Vec3 n0, n1, n2;
                if (fv[0].normalIdx != -1) {
                    n0 = normals[fv[0].normalIdx];
                    n1 = normals[fv[1].normalIdx];
                    n2 = normals[fv[2].normalIdx];
                } else {
                    Vec3 faceNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
                    n0 = n1 = n2 = faceNormal;
                }
            */
        }
    }

    bvh = make_unique<BVH>(triangles);
}

void Mesh::addTriangle(shared_ptr<Triangle> triangle) {
    triangles.push_back(triangle);
}

void Mesh::addTriangle(const Vec3& a, const Vec3& b, const Vec3&c) {
    triangles.push_back(make_shared<Triangle>(a, b, c, mat));
}

bool Mesh::hit(const Ray& ray, float tmin, float tmax, HitRecord& record) const {
    return bvh->hit(ray, tmin, tmax, record, bvh->root);
}

AABB Mesh::bbox() const {
    return bvh->bbox();
}