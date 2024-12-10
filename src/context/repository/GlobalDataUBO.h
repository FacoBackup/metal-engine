#ifndef GLOBALDATAUBO_H
#define GLOBALDATAUBO_H
#include <glm/mat4x4.hpp>

namespace Metal {
    struct GlobalDataUBO {
        glm::mat4x4 view{};
        glm::mat4x4 proj{};
        glm::mat4x4 viewProj{};
        glm::mat4x4 invView{};
        glm::mat4x4 invProj{};
        glm::vec4 color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
    };
}

#endif
