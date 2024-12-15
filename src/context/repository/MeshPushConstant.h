#ifndef MESHPUSHCONSTANT_H
#define MESHPUSHCONSTANT_H
#include <glm/mat4x4.hpp>

namespace Metal {
    struct MeshPushConstant final {
        glm::mat4x4 model;
        glm::mat4x4 data;
    };
}
#endif //MESHPUSHCONSTANT_H
