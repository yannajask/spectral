#pragma once

#include "Light.h"
#include "../utils.h"

class LightDistribution {
    public:
        LightDistribution(const std::vector<shared_ptr<Light>>& lights) {
            float sum = 0.0f;

            for (const auto& light : lights) {
                sum += light->power();
                cdf.push_back(sum);
            }

            if (sum > 0.0f) {
                for (float &p : cdf) {
                    p /= sum;
                }
            }
        }

        int sample(float& pdf) const {
            float r = randf(0.0f, 1.0f);
            int i = std::lower_bound(cdf.begin(), cdf.end(), r) - cdf.begin();
            i = std::min(i, int(cdf.size()) - 1);
            pdf = (i == 0) ? cdf[0] : cdf[i] - cdf[i - 1];
            return i;
        }

    private:
        std::vector<float> cdf;
};