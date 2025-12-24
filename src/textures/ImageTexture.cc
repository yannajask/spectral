#include "ImageTexture.h"

ImageTexture::ImageTexture(shared_ptr<Image> image) : image(image) {}

Vec3 ImageTexture::evaluate(float u, float v, const Vec3& p) const {
    return image->evaluate(u, v);
}