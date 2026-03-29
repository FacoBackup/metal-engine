#ifndef METAL_ENGINE_COMMANDBUFFERRECORDERBUILDER_H
#define METAL_ENGINE_COMMANDBUFFERRECORDERBUILDER_H

#include "ResourceBuilder.h"

namespace Metal {
    class CommandBufferRecorderService;
    class RenderTargetService;

    class CommandBufferRecorderBuilder final : public ResourceBuilder {
        std::string renderTargetId;
        bool clearBuffer;
        bool computeMode;
        RenderTargetService *RenderTargetService = nullptr;
        CommandBufferRecorderService *commandBufferRecorderService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"RenderTargetService", &RenderTargetService},
                {"CommandBufferRecorderService", &commandBufferRecorderService}
            };
        }

        explicit CommandBufferRecorderBuilder(const std::string &id, std::string renderTargetId, bool clearBuffer = true)
            : ResourceBuilder(id), renderTargetId(std::move(renderTargetId)), clearBuffer(clearBuffer),
              computeMode(false) {
        }

        explicit CommandBufferRecorderBuilder(const std::string &id)
            : ResourceBuilder(id), renderTargetId(""), clearBuffer(false), computeMode(true) {
        }

        ResourceType getType() override {
            return COMMAND_BUFFER_RECORDER;
        }

        RuntimeResource *build() override;

        bool isComputeMode() const { return computeMode; }
        const std::string &getrenderTargetId() const { return renderTargetId; }
        bool shouldClearBuffer() const { return clearBuffer; }
    };
}

#endif
