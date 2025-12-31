#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <cmath>
#include <limits>

using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Mat3 = glm::mat3;

using std::make_shared;
using std::shared_ptr;

using std::make_unique;
using std::unique_ptr;

inline thread_local std::mt19937 rng(std::random_device{}());
inline thread_local std::uniform_real_distribution<float> distribution(0.0, 1.0);

constexpr float infinity = std::numeric_limits<float>::infinity();
constexpr float pi = 3.1415927;

inline float randf() {
    return distribution(rng);
}

inline float randf(float min, float max) {
    return min + (max - min) * randf();
}

inline int randi(int min, int max) {
    return int(randf(min, max + 1));
}

inline float GTR1(float cosTheta, float a) {
    if (a >= 1.0f) return 1.0f / pi;
    float a2 = a * a;
    return (a2 - 1.0f) / (pi * std::log(a2) * (1.0f + (a2 - 1.0f) * cosTheta * cosTheta));
}

inline float GTR2(float cosTheta, float a) {
    if (a >= 1.0f) return 1.0f / pi;
    float a2 = a * a;
    float denom = 1.0f + (a2 - 1.0f) * cosTheta * cosTheta;
    return a2 / (pi * denom * denom);
}

inline float anisotropicGTR2(const Vec3& hl, float ax, float ay) {
    float x = (hl.x * hl.x) / (ax * ax);
    float y = (hl.y * hl.y) / (ay * ay);
    float z = (hl.z * hl.z);
    float denom = x + y + z;
    return 1.0f / (pi * ax * ay * denom * denom);
}

inline float anisotropicSmithGGX(const Vec3& v, float ax, float ay) {
    float t = (v.x * v.x * ax * ax + v.y * v.y * ay * ay) / (v.z * v.z);
    float lambda = (std::sqrt(1.0f + t) - 1.0f) / 2.0f;
    return 1.0f / (1.0f + lambda);
}

inline float smithGGX(const Vec3& v, float a) {
    return anisotropicSmithGGX(v, a, a);
}

// https://graphics.pixar.com/library/OrthonormalB/paper.pdf
constexpr Mat3 worldToLocal3x3(const Vec3& n) {
    float sign = std::copysign(1.0f, n.z);
    const float a = -1.0f / (sign + n.z);
    const float b = n.x * n.y * a;
    Vec3 b1 = Vec3(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
    Vec3 b2 = Vec3(b, sign + n.y * n.y * a, -n.y);
    return glm::transpose(Mat3(b1, b2, n));
}

inline Vec2 rand2f() {
    return Vec2(randf(), randf());
}

inline Vec2 rand2f(float min, float max) {
    return Vec2(randf(min, max), randf(min, max));
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

inline Vec3 rand3fhs(const Vec3& n) {
    Vec3 unit = rand3funit();
    if (glm::dot(unit, n) > 0.0f) {
        return unit;
    } else {
        return -unit;
    }
}

// to do: remove matrix multiplication
inline Vec3 rand3fcd(const Vec3& n) {
    Vec2 u = rand2f(0.0f, 1.0f);
    float r = std::sqrt(u.x);
    float phi = 2.0f * pi * u.y;
    Mat3 T = worldToLocal3x3(n);
    return T * Vec3(r * std::cos(phi), r * std::sin(phi), std::sqrt(1.0f - u.x));
}

inline Vec3 sampleSquare() {
    return Vec3(randf() - 0.5f, randf() - 0.5f, 0.0f);
}

constexpr float radians(float degrees) {
    return degrees * (pi / 180.0f);
}

inline float schlick(float cosTheta, float F0) {
    cosTheta = std::clamp(cosTheta, 0.0f, 1.0f);
    float m = 1.0f - cosTheta;
    float m5 = m * m * m * m * m;
    return F0 + (1.0f - F0) * m5;
}

inline Vec3 schlick(float cosTheta, const Vec3& F0) {
    cosTheta = std::clamp(cosTheta, 0.0f, 1.0f);
    float m = 1.0f - cosTheta;
    float m5 = m * m * m * m * m;
    return F0 + (Vec3(1.0f) - F0) * m5;
}

constexpr float fresnel(float cosThetaI, float eta) {
    const float sinThetaT2 = eta * eta * (1.0f - cosThetaI * cosThetaI);
    if (sinThetaT2 > 1.0f) return 1.0f;
    const float cosThetaT = std::sqrt(std::max(0.0f, 1.0f - sinThetaT2));
    const float Rs = (cosThetaI - eta * cosThetaT) / (cosThetaI + eta * cosThetaT);
    const float Rp = (eta * cosThetaI - cosThetaT) / (eta * cosThetaI + cosThetaT);
    return 0.5f * (Rs * Rs + Rp * Rp);
}

constexpr Vec3 reflect(const Vec3& v, const Vec3& n) {
    return v - 2 * glm::dot(v, n) * n;
}

inline Vec3 refract(const Vec3& uv, const Vec3& n, float ratio) {
    float cosTheta = std::fmin(dot(-uv, n), 1.0f);
    Vec3 perp = ratio * (uv + cosTheta * n);
    Vec3 parallel = -std::sqrt(std::fabs(1.0f - glm::length2(perp))) * n;
    return perp + parallel;
}

#include "geometry/Ray.h"