#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <cmath>
#include <limits>

typedef glm::vec3 Vec3;
typedef glm::vec4 Vec4;
typedef glm::mat4x4 Mat4x4;

constexpr double infinity = std::numeric_limits<double>::infinity();

constexpr float radians(float degrees) {
    return degrees * (M_PI / 180.0f);
}

inline Vec3 transformDirMatrix(const Mat4x4& A, const Vec3& v) {
    Vec4 tmp = glm::normalize(A * Vec4(v, 0.0f));
    return Vec3(tmp.x, tmp.y, tmp.z);
}

inline Vec3 transformPointMatrix(const Mat4x4& A, const Vec3& v) {
    Vec4 tmp = A * Vec4(v, 1.0f);
    return Vec3(tmp.x, tmp.y, tmp.z);
}