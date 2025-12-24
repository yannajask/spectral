#pragma once

#include "Texture.h"
#include "Image.h"

class ImageTexture : public Texture {
    public:
        explicit ImageTexture(shared_ptr<Image> image);
        virtual Vec3 evaluate(float u, float v, const Vec3& p) const;

    private:
        shared_ptr<Image> image;
};