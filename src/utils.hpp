#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <random>
#include <cmath>
#include <limits>

typedef glm::vec3 Vec3;
typedef glm::vec4 Vec4;
typedef glm::mat4x4 Mat4x4;

using std::make_shared;
using std::shared_ptr;

inline thread_local std::mt19937 rng(std::random_device{}());
inline thread_local std::uniform_real_distribution<float> distribution(0.0, 1.0);

inline float randf() {
    return distribution(rng);
}

inline float randf(float min, float max) {
    return min + (max - min) * randf();
}

inline int randi(int min, int max) {
    return int(randf(min, max + 1));
}

inline Vec3 sampleSquare() {
    return Vec3(randf() - 0.5f, randf() - 0.5f, 0.0f);
}

constexpr float infinity = std::numeric_limits<float>::infinity();

constexpr float radians(float degrees) {
    return degrees * (M_PI / 180.0f);
}

constexpr Vec3 transformDirMatrix(const Mat4x4& A, const Vec3& v) {
    Vec4 tmp = glm::normalize(A * Vec4(v, 0.0f));
    return Vec3(tmp.x, tmp.y, tmp.z);
}

constexpr Vec3 transformPointMatrix(const Mat4x4& A, const Vec3& v) {
    Vec4 tmp = A * Vec4(v, 1.0f);
    return Vec3(tmp.x, tmp.y, tmp.z);
}

#include "Ray.hpp"