#ifndef DENOISER_PUSH_CONSTANT_H
#define DENOISER_PUSH_CONSTANT_H
#include <glm/mat4x4.hpp>

namespace Metal {
    struct DenoiserPC {
        alignas(4) float diffWeight;
    };
}
#endif
