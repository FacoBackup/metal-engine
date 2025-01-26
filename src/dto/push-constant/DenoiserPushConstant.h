#ifndef DENOISER_PUSH_CONSTANT_H
#define DENOISER_PUSH_CONSTANT_H

namespace Metal {
    struct DenoiserPushConstant {
        alignas(4) float normalPhi,  stepWidth;
    };
}
#endif
