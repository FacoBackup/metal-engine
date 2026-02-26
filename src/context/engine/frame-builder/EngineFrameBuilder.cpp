#include "EngineFrameBuilder.h"

#include "structures/FramebufferBuilder.h"
#include "structures/TextureBuilder.h"
#include "EngineFrame.h"

namespace Metal {
    EngineFrameBuilder &EngineFrameBuilder::addFramebuffer(std::string id, const unsigned w, const unsigned h,
                                                           glm::vec4 clearColor) {
        currentBuilder = std::make_shared<FramebufferBuilder>(id, w, h, clearColor);
        builders.push_back(currentBuilder);
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addColor(std::string id, VkFormat format, VkImageUsageFlagBits usage,
                                                     FrameBufferInstance *framebuffer) {
        dynamic_cast<FramebufferBuilder *>(currentBuilder.get())->addColor(id, format, usage, framebuffer);
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addDepth() {
        dynamic_cast<FramebufferBuilder *>(currentBuilder.get())->addDepth();
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addTexture(const std::string &id, unsigned w, unsigned h) {
        currentBuilder = std::make_shared<TextureBuilder>(id, w, h);
        builders.push_back(currentBuilder);
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addTexture(const std::string &id) {
        if (!tryMatch(id, ResourceBuilderType::TEXTURE)) {
            throw std::runtime_error("Texture not found");
        }
        return *this;
    }

    bool EngineFrameBuilder::tryMatch(const std::string &id, ResourceBuilderType type) {
        for (std::shared_ptr<ResourceBuilder> &builder: builders) {
            if (builder->getId() == id && builder->getType() == type) {
                currentBuilder = builder;
                return true;
            }
        }
        return false;
    }

    std::unique_ptr<EngineFrame> EngineFrameBuilder::build() {
        auto frame = std::make_unique<EngineFrame>();
        for (const auto &builder: builders) {
            frame->addResource(builder->build());
        }
        return frame;
    }
} // Metal
