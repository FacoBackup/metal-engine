#ifndef MESHPUSHCONSTANT_H
#define MESHPUSHCONSTANT_H
#include <glm/mat4x4.hpp>

namespace Metal {
    struct MeshPushConstant final {
        glm::mat4x4 model;
        alignas(4) unsigned int renderIndex;
    };
}
#endif //MESHPUSHCONSTANT_H
