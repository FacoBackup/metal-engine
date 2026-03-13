#ifndef ACCUMULATION_H
#define ACCUMULATION_H
#include <glm/mat4x4.hpp>

namespace Metal {
    struct AccumulationPushConstant {
        alignas(16) glm::mat4 previousFrameProjView;
    };
}
#endif
