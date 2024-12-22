#ifndef GBUFFER_SHADING_PUSH_CONSTANT_H
#define GBUFFER_SHADING_PUSH_CONSTANT_H
#include <glm/mat4x4.hpp>

namespace Metal {
    struct GBufferShadingPushConstant final {
        int mode;
    };
}
#endif //GBUFFER_SHADING_PUSH_CONSTANT_H
