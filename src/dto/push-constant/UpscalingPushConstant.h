#ifndef UPSCALINGPUSHCONSTANT_H
#define UPSCALINGPUSHCONSTANT_H
#include <glm/vec2.hpp>

namespace Metal {
    struct UpscalingPushConstant {
        alignas(4) float upscaleFactor;
        alignas(8) glm::vec2 targetImageSize;
    };
}

#endif //UPSCALINGPUSHCONSTANT_H
