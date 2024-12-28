#ifndef ICONPUSHCONSTANT_H
#define ICONPUSHCONSTANT_H
#include <glm/vec3.hpp>

namespace Metal {
    struct IconPushConstant final {
        alignas(16) glm::vec3 translation;
        alignas(16) glm::vec3 iconColor;
        alignas(4)float imageIndex;
        alignas(4)bool isSelected;
        alignas(4) float iconScale;
    };
}
#endif //ICONPUSHCONSTANT_H
