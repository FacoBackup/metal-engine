#ifndef TILEINFOUBO_H
#define TILEINFOUBO_H
#include <glm/vec3.hpp>
#include <array>

namespace Metal {
    struct TileInfoUBO final {
        std::array<glm::vec4, 9> tileCenterValid{};
    };
}

#endif //TILEINFOUBO_H
