#ifndef USER_INTERFACE_PC_H
#define USER_INTERFACE_PC_H
#include <glm/vec2.hpp>

namespace Metal {
    struct UserInterfacePC {
        alignas(8) glm::vec2 viewportPos;
        alignas(4) float shadowSize;
        alignas(4) float borderRadius;
        alignas(8) glm::vec2 viewportSize;
        alignas(4) int blurStrength; // NEW
        alignas(4) bool isDarkMode; // NEW
        alignas(4) bool isCompact;
        alignas(4) int windowCount;
        alignas(8) glm::vec2 windowAspectRatio;

    };
}
#endif //USER_INTERFACE_PC_H
