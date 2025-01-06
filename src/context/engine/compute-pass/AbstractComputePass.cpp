#include "AbstractComputePass.h"
#include "../../../context/ApplicationContext.h"
#include "../../../service/framebuffer/FrameBufferInstance.h"

namespace Metal {
    AbstractComputePass::AbstractComputePass(ApplicationContext &context) : AbstractPass(context, true) {
    }

    void AbstractComputePass::recordDispatch(const unsigned int groupX, const unsigned int groupY,
                                             const unsigned int groupZ) const {
        vkCmdDispatch(vkCommandBuffer, groupX, groupY, groupZ);
    }

    void AbstractComputePass::recordImageDispatch(const FrameBufferInstance *fb, const unsigned int threadCountX,
                                                  const unsigned int threadCountY) const {
        vkCmdDispatch(vkCommandBuffer, fb->bufferWidth / threadCountX, fb->bufferHeight / threadCountY, 1);
    }
}
