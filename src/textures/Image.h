#pragma once

#include "../utils.h"
#include "../extern/stb/stb_image.h"

struct STBClose {
    void operator()(float* p) noexcept { if (p) stbi_image_free(p); }
};

using float_pointer = std::unique_ptr<float[], STBClose>;

class Image {
    public:
        explicit Image(std::string& filePath);

        Vec3 evaluate(float u, float v) const;

    private:
        int width, height;
        float_pointer fdata = nullptr;

        const int bytesPerPixel = 3;
        int bytesPerScanline;

        static int clamp(int x, int low, int high);
};

Image::Image(std::string& filePath) {
    auto n = bytesPerPixel;
    float* data = stbi_loadf(filePath.c_str(), &width, &height, &n, bytesPerPixel);
    if (data) {
        fdata = float_pointer(data);
        bytesPerScanline = width * bytesPerPixel;
    } else {
        width = height = bytesPerScanline = 0;
    }
}

Vec3 Image::evaluate(float u, float v) const {
    if (!fdata) return Vec3(1.0f, 0.0f, 1.0f); // return magenta

    int i = static_cast<int>(u * width);
    int j = static_cast<int>((1.0f - v) * height);
    i = clamp(u, 0, width);
    j = clamp(v, 0, height);

    const float* pixel = fdata.get() + (j * bytesPerScanline) + (i * bytesPerPixel);
    return Vec3(pixel[0], pixel[1], pixel[2]);
}

int Image::clamp(int x, int low, int high) {
    if (x < low) {
        return low;
    } else if (x < high) {
        return x;
    } else {
        return high - 1;
    }
}