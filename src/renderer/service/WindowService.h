#ifndef WINDOWSERVICE_H
#define WINDOWSERVICE_H
#include <array>

#include "../../common/engine-definitions.h"
#include "../data/WindowBuffer.h"

namespace Metal {
    class WindowService final {
        std::array<WindowBuffer, MAX_WINDOWS> windows{};
        int currentWindow = 0;

    public:
        void reset() {
            currentWindow = 0;
        }

        void beginWindow(glm::vec2 pos, glm::vec2 size);

        void updateBuffer();

        int getCurrentWindow() const {
            return currentWindow;
        }
    };
} // Metal

#endif //WINDOWSERVICE_H
