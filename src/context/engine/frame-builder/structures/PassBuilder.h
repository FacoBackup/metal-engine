#ifndef METAL_ENGINE_PASSBUILDER_H
#define METAL_ENGINE_PASSBUILDER_H

#include "ResourceBuilder.h"
#include "../../../../enum/PassType.h"
#include "../../../../enum/ResourceType.h"

namespace Metal {
    class PassBuilder final : public ResourceBuilder {
        PassType passType;
        std::string commandBufferId;
        std::string framebufferId;
        bool clearBuffer;

    public:
        explicit PassBuilder(const std::string &id, PassType type, std::string commandBufferId,
                             std::string framebufferId = "", bool clearBuffer = true)
            : ResourceBuilder(id), passType(type), commandBufferId(std::move(commandBufferId)),
              framebufferId(std::move(framebufferId)), clearBuffer(clearBuffer) {
        }

        ResourceType getType() override {
            return PASS;
        }

        RuntimeResource *build() override;

        [[nodiscard]] PassType getPassType() const { return passType; }
        const std::string &getCommandBufferId() const { return commandBufferId; }
        const std::string &getFramebufferId() const { return framebufferId; }
        bool shouldClearBuffer() const { return clearBuffer; }
    };
}

#endif
