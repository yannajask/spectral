#pragma once

#include "../utils.h"

class Light {
    public:
        virtual ~Light() = default;

    protected:
        Vec3 colour;
};

class DirectionalLight : public Light {
    public:


    private:
        Vec3 dir;
};

class PointLight : public Light {
    public:
    
    
    private:
        Vec3 position;
};