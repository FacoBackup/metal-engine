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
        vkCmdDispatch(vkCommandBuffer, image->width / threadCountX, image->height / threadCountY, 1);
    }
}
