#ifndef SELECTEDPUSHCONSTANT_H
#define SELECTEDPUSHCONSTANT_H
#include <glm/mat4x4.hpp>

namespace Metal {
    struct SelectionOutlinePushConstant final {
        alignas(16) glm::mat4x4 model;
        alignas(16) glm::vec4 selectionColor;
        alignas(4) unsigned int renderIndex;
    };
}
#endif
