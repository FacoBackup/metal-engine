#ifndef GLOBALDATAUBO_H
#define GLOBALDATAUBO_H
#include <glm/mat4x4.hpp>

namespace Metal {
    struct GlobalDataUBO {
        alignas(16) glm::mat4x4 view{};
        alignas(16) glm::mat4x4 proj{};
        alignas(16) glm::mat4x4 viewProj{};
        alignas(16) glm::mat4x4 invView{};
        alignas(16) glm::mat4x4 invProj{};
        alignas(16) glm::vec4 color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    };
}

#endif
