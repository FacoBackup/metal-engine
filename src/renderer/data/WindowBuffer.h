#ifndef WINDOWBUFFER_H
#define WINDOWBUFFER_H
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Metal {
    struct WindowBuffer final {
        alignas(8) glm::vec2 pos{};
        alignas(8) glm::vec2 size{};
    };
}

#endif //WINDOWBUFFER_H
