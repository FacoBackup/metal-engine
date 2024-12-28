#ifndef TILEINFOUBO_H
#define TILEINFOUBO_H
#include <glm/vec3.hpp>

namespace Metal {
    struct TileInfoUBO {
        alignas(16) glm::vec3 tileCenter;
    };
}

#endif //TILEINFOUBO_H
