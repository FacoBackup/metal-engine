#ifndef TILEINFOUBO_H
#define TILEINFOUBO_H
#include <glm/vec3.hpp>
#include <array>

namespace Metal {
    struct TileInfoUBO final {
        std::array<glm::vec4, 9> tileCenterValid{};
        std::array<unsigned int, 9> voxelBufferOffset{};
    };
}

#endif //TILEINFOUBO_H
