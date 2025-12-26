#pragma once

#include "utils.h"
#include <charconv>
#include <string_view>
#include <system_error>

// adapted from https://github.com/scratchapixel/scratchapixel-code/blob/main/obj-file-format/objimporter.cc

struct FaceVertex {
    int vertexIdx = -1;
    int uvIdx     = -1;
    int normalIdx = -1;
};

struct FaceGroup {
    std::vector<FaceVertex> faceVertices;
    std::string name;
};

void parseFaceVertex(std::string_view tuple, FaceVertex& fv) {
    const char* ptr = tuple.data();
    const char* end = ptr + tuple.size();

    auto [p1, ec1] = std::from_chars(ptr, end, fv.vertexIdx);
    if (ec1 == std::errc()) fv.vertexIdx -= 1;

    ptr = p1;
    if (ptr >= end || *ptr != '/') return;
    ptr++;

    if (ptr < end && *ptr != '/') {
        auto [p2, ec2] = std::from_chars(ptr, end, fv.uvIdx);
        if (ec2 == std::errc()) fv.uvIdx -= 1;
        ptr = p2;
    }

    if (ptr >= end || *ptr != '/') return;
    ptr++;

    if (ptr < end) {
        auto [p3, ec3] = std::from_chars(ptr, end, fv.normalIdx);
        if (ec3 == std::errc()) fv.normalIdx -= 1;
    }
}
