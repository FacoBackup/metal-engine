#include "AbstractComputePass.h"
#include "../../../context/ApplicationContext.h"
#include "../../../service/texture/TextureInstance.h"

namespace Metal {
    AbstractComputePass::AbstractComputePass(ApplicationContext &context) : AbstractPass(context, true) {
    }

    void AbstractComputePass::recordDispatch(const unsigned int groupX, const unsigned int groupY,
                                             const unsigned int groupZ) const {
        vkCmdDispatch(vkCommandBuffer, groupX, groupY, groupZ);
    }

    void AbstractComputePass::recordImageDispatch(const TextureInstance *image, const unsigned int threadCountX,
                                                  const unsigned int threadCountY) const {
        unsigned int groupX = (image->width + threadCountX - 1) / threadCountX;
        unsigned int groupY = (image->height + threadCountY - 1) / threadCountY;
        vkCmdDispatch(vkCommandBuffer, groupX, groupY, 1);
    }
}
