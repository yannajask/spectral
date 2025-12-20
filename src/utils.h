#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <cmath>
#include <limits>

typedef glm::vec3 Vec3;
typedef glm::vec4 Vec4;
typedef glm::mat4x4 Mat4x4;

using std::make_shared;
using std::shared_ptr;

using std::make_unique;
using std::unique_ptr;

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

inline Vec3 rand3f() {
    return Vec3(randf(), randf(), randf());
}

inline Vec3 rand3f(float min, float max) {
    return Vec3(randf(min, max), randf(min, max), randf(min, max));
}

inline Vec3 rand3funit() {
    while (true) {
        Vec3 p = rand3f(-1, 1);
        float lensq = glm::length2(p);
        if (1e-160 < lensq && lensq <= 1) return p / sqrt(lensq);
    }
}

inline Vec3 rand3fhs(const Vec3& normal) {
    Vec3 unit = rand3funit();
    if (glm::dot(unit, normal) > 0.0f) {
        return unit;
    } else {
        return -unit;
    }
}

inline Vec3 sampleSquare() {
    return Vec3(randf() - 0.5f, randf() - 0.5f, 0.0f);
}

constexpr float infinity = std::numeric_limits<float>::infinity();

constexpr float radians(float degrees) {
    return degrees * (M_PI / 180.0f);
}

#include "geometry/Ray.h"