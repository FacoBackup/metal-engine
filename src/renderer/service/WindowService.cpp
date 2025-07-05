#include "WindowService.h"
#include "../../MetalContextProvider.h"

namespace Metal {
    void WindowService::beginWindow(glm::vec2 pos, glm::vec2 size) {
        windows[currentWindow].pos = pos;
        windows[currentWindow].size = size;
        currentWindow++;
    }

    void WindowService::updateBuffer() {
        SINGLETONS.coreBuffers.windowBuffer->update(windows.data());
    }
} // Metal
