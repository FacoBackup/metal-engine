#include "AbstractComputePass.h"
#include "../../../context/ApplicationContext.h"

namespace Metal {
    AbstractComputePass::AbstractComputePass(ApplicationContext &context) : AbstractPass(context, true) {
    }

    void AbstractComputePass::recordDispatch(const unsigned int groupX, const unsigned int groupY,
                                             const unsigned int groupZ) const {
        vkCmdDispatch(vkCommandBuffer, groupX, groupY, groupZ);
    }
}
