#ifndef RUNTIMEREPOSITORY_H
#define RUNTIMEREPOSITORY_H
#include <glm/vec2.hpp>

namespace Metal {
    struct RuntimeRepository {

        bool isFocused;
        int displayW;
        int displayH;

        glm::vec2 invResolution{};
        bool fasterPressed = false;
        bool forwardPressed = false;
        bool backwardPressed = false;
        bool leftPressed = false;
        bool rightPressed = false;
        bool upPressed = false;
        bool downPressed = false;
        bool mousePressed;
        float mouseX = 0;
        float mouseY = 0;
        float normalizedMouseX = 0;
        float normalizedMouseY = 0;
        float viewportW = 0;
        float viewportH = 0;
        float viewportX = 0;
        float viewportY = 0;
    };
}
#endif //RUNTIMEREPOSITORY_H
