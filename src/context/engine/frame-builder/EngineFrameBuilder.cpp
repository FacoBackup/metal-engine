#include "EngineFrameBuilder.h"

#include "structures/FramebufferBuilder.h"

namespace Metal {
    EngineFrameBuilder &EngineFrameBuilder::addFramebuffer(std::string id) {
        currentBuilder = std::make_shared<FramebufferBuilder>(id);
        builders.push_back(currentBuilder);
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addColor(std::string id, VkFormat format, VkImageUsageFlagBits usage,
                                                     FrameBufferInstance *framebuffer) {
        static_cast<FramebufferBuilder*>(currentBuilder.get())->addColor(id, format, usage, framebuffer);
        return *this;
    }
} // Metal
