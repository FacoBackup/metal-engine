#ifndef ICONPUSHCONSTANT_H
#define ICONPUSHCONSTANT_H
#include <glm/vec3.hpp>

namespace Metal {
    struct IconPushConstant final {
        alignas(16) glm::vec3 iconColor;
    };
}
#endif //ICONPUSHCONSTANT_H
