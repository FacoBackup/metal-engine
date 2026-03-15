#include "CommandBufferRecorderBuilder.h"
#include "../../service/CommandBufferRecorderService.h"
#include "../../service/FrameBufferService.h"

namespace Metal {
    RuntimeResource *CommandBufferRecorderBuilder::build() {
        if (computeMode) {
            return commandBufferRecorderService->createCompute(id);
        }
        auto *fbo = framebufferService->getResource(framebufferId);
        return commandBufferRecorderService->create(id, fbo, clearBuffer);
    }
}
