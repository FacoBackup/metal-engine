#ifndef RENDERTARGET_INSTANCE_H
#define RENDERTARGET_INSTANCE_H

#include "RuntimeResource.h"
#include <vector>
#include <glm/vec4.hpp>
#include "RenderTargetAttachment.h"

namespace Metal {
    struct RenderTargetInstance final : RuntimeResource {
        unsigned int bufferWidth{};
        unsigned int bufferHeight{};
        glm::vec4 clearColor;
        std::vector<std::shared_ptr<RenderTargetAttachment> > attachments{};

        explicit RenderTargetInstance(const std::string &id) : RuntimeResource(id) {
        }

        ResourceType resourceType() override {
            return RENDER_TARGET;
        }
    };
}

#endif
