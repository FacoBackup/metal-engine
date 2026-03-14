#ifndef METAL_ENGINE_COMMANDBUFFERRECORDERBUILDER_H
#define METAL_ENGINE_COMMANDBUFFERRECORDERBUILDER_H

#include "ResourceBuilder.h"

namespace Metal {
    class CommandBufferRecorderService;
    class FrameBufferService;

    class CommandBufferRecorderBuilder final : public ResourceBuilder {
        std::string framebufferId;
        bool clearBuffer;
        bool computeMode;
        FrameBufferService *framebufferService = nullptr;
        CommandBufferRecorderService *commandBufferRecorderService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"FrameBufferService", framebufferService},
                {"CommandBufferRecorderService", commandBufferRecorderService}
            };
        }

        explicit CommandBufferRecorderBuilder(const std::string &id, std::string framebufferId, bool clearBuffer = true)
            : ResourceBuilder(id), framebufferId(std::move(framebufferId)), clearBuffer(clearBuffer),
              computeMode(false) {
        }

        explicit CommandBufferRecorderBuilder(const std::string &id)
            : ResourceBuilder(id), framebufferId(""), clearBuffer(false), computeMode(true) {
        }

        ResourceType getType() override {
            return COMMAND_BUFFER_RECORDER;
        }

        RuntimeResource *build() override;

        bool isComputeMode() const { return computeMode; }
        const std::string &getFramebufferId() const { return framebufferId; }
        bool shouldClearBuffer() const { return clearBuffer; }
    };
}

#endif
