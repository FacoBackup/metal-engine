#include "CommandBufferRecorderBuilder.h"
#include "../../service/CommandBufferRecorderService.h"
#include "../../service/RenderTargetService.h"

namespace Metal {
    RuntimeResource *CommandBufferRecorderBuilder::build() {
        if (computeMode) {
            return commandBufferRecorderService->createCompute(id);
        }
        auto *fbo = RenderTargetService->getResource(renderTargetId);
        return commandBufferRecorderService->create(id, fbo, clearBuffer);
    }
}
