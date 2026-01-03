#pragma once

#include "../utils.h"
#include "../geometry/Object.h"

struct LightSample {
    Vec3 Li;
    Vec3 wo;
    float dist;
    float pdf;
    bool isDelta; // will use later
};

class Light {
    public:
        Light(const Vec3& colour, float lumens) : colour(colour), luminance(lumens) {}
        virtual ~Light() = default;

        virtual LightSample sample(const HitRecord& record) const = 0;
        virtual float power() const = 0;

    protected:
        Vec3 colour;
        float luminance;
};

class DirectionalLight : public Light {
    public:
        DirectionalLight(const Vec3& colour, float lumens, const Vec3& dir) : Light(colour, lumens), dir(dir) {}
        virtual LightSample sample(const HitRecord& record) const override;
        virtual float power() const override;

    private:
        Vec3 dir;
};

class PointLight : public Light {
    public:
        PointLight(const Vec3& colour, float lumens, const Vec3& p) : Light(colour, lumens), position(p) {}
        virtual LightSample sample(const HitRecord& record) const override;
        virtual float power() const override;

    private:
        Vec3 position;
};

class InfiniteLight : public Light {
    public:
        InfiniteLight(const Vec3& colour, float lumens): Light(colour, lumens) {}
        virtual LightSample sample(const HitRecord& record) const override;
        virtual float power() const override;  
};