#include "CommandBufferRecorderBuilder.h"
#include "../../../ApplicationContext.h"

namespace Metal {
    RuntimeResource *CommandBufferRecorderBuilder::build() {
        if (computeMode) {
            return CTX.commandBufferRecorderService.createCompute(id);
        }
        auto *fbo = CTX.framebufferService.getResource(framebufferId);
        return CTX.commandBufferRecorderService.create(id, fbo, clearBuffer);
    }
}
