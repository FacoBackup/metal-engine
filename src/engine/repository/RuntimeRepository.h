#ifndef RUNTIMEREPOSITORY_H
#define RUNTIMEREPOSITORY_H
#include <glm/vec2.hpp>

namespace Metal {
    struct RuntimeRepository : IContextMember {
        bool isFocused;
        int displayW;
        int displayH;

        glm::vec2 invResolution{};
        bool mousePressed;
        float mouseX = 0;
        float mouseY = 0;
        float normalizedMouseX = 0;
        float normalizedMouseY = 0;
        float viewportW = 1;
        float viewportH = 1;
        float viewportX = 0;
        float viewportY = 0;
    };
}
#endif //RUNTIMEREPOSITORY_H
