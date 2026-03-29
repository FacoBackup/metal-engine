#include "EngineFrameBuilder.h"

#include "../../core/VulkanContext.h"
#include "structures/RenderTargetBuilder.h"
#include "structures/TextureBuilder.h"
#include "structures/BufferBuilder.h"
#include "structures/CommandBufferRecorderBuilder.h"
#include "EngineFrame.h"
#include "../../ApplicationContext.h"
#include "../passes/CommandBufferRecorder.h"
#include "../EngineContext.h"

namespace Metal {
    EngineFrameBuilder::EngineFrameBuilder(std::string frameId) : frameId(std::move(frameId)) {
    }

    EngineFrameBuilder &EngineFrameBuilder::addRenderTarget(std::string id, const unsigned w, const unsigned h,
                                                           glm::vec4 clearColor) {
        currentBuilder = std::make_shared<RenderTargetBuilder>(frameId + "_" + id, w, h, clearColor);
        storeBuilder();
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addRenderTarget(const std::string &id) {
        if (!tryMatch(frameId + "_" + id, ResourceType::RENDER_TARGET)) {
            throw std::runtime_error("RenderTarget not found");
        }
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addColor(VkFormat format, VkImageUsageFlags usage) {
        dynamic_cast<RenderTargetBuilder *>(currentBuilder.get())->addColor(format, usage, nullptr);
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addDepth() {
        dynamic_cast<RenderTargetBuilder *>(currentBuilder.get())->addDepth();
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addTexture(const std::string &id, unsigned w, unsigned h, VkFormat format) {
        currentBuilder = std::make_shared<TextureBuilder>(frameId + "_" + id, w, h, format);
        storeBuilder();
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addTexture(const std::string &id) {
        if (!tryMatch(frameId + "_" + id, ResourceType::TEXTURE)) {
            throw std::runtime_error("Texture not found");
        }
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addBuffer(const std::string &id, VkDeviceSize size,
                                                      VkMemoryPropertyFlags properties, BufferType type) {
        currentBuilder = std::make_shared<BufferBuilder>(
            frameId + "_" + id, size, type == UNIFORM_BUFFER
                                          ? VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
                                          : VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            properties, type);
        storeBuilder();
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addBuffer(const std::string &id) {
        if (!tryMatch(frameId + "_" + id, ResourceType::BUFFER)) {
            throw std::runtime_error("Buffer not found");
        }
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addCommandBuffer(const std::string &id, const std::string &renderTargetId,
                                                             const bool clearBuffer) {
        currentBuilder = std::make_shared<CommandBufferRecorderBuilder>(
            frameId + "_" + id, frameId + "_" + renderTargetId, clearBuffer);
        storeBuilder();
        return *this;
    }

    void EngineFrameBuilder::storeBuilder() {
        ctx->injectDependencies(currentBuilder.get());
        builders.push_back(currentBuilder);
    }

    EngineFrameBuilder &EngineFrameBuilder::addComputeCommandBuffer(const std::string &id, bool requiresRayTracing) {
        if (requiresRayTracing && !vulkanContext->isRayTracingSupported()) {
            return *this;
        }
        currentBuilder = std::make_shared<CommandBufferRecorderBuilder>(frameId + "_" + id);
        storeBuilder();
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addPass(std::unique_ptr<AbstractPass> pass,
                                                    const std::string &commandBufferId) {
        if (pass->requiresRayTracing() && !vulkanContext->isRayTracingSupported()) {
            return *this;
        }
        passes.emplace_back(std::move(pass), frameId + "_" + commandBufferId);
        return *this;
    }

    bool EngineFrameBuilder::tryMatch(const std::string &id, ResourceType type) {
        for (std::shared_ptr<ResourceBuilder> &builder: builders) {
            if (builder->getId() == id && builder->getType() == type) {
                currentBuilder = builder;
                return true;
            }
        }
        return false;
    }

    void EngineFrameBuilder::build() {
        auto *frame = new EngineFrame(frameId);
        engineContext->registerFrame(frame);
        std::unordered_map<std::string, RuntimeResource *> builtResources;

        for (const auto &builder: builders) {
            if (auto *resource = builder->build()) {
                frame->addResource(resource);
                builtResources[builder->getId()] = resource;
            }
        }

        std::unordered_map<std::string, std::vector<std::unique_ptr<AbstractPass> > > recorderToPasses;
        std::vector<std::string> recorderOrder;

        for (auto &pass: passes) {
            recorderOrder.push_back(pass.commandBufferId);

            pass.pass->frame = frame;
            ctx->injectDependencies(pass.pass.get());
            pass.pass->onInitialize();
            recorderToPasses[pass.commandBufferId].push_back(std::move(pass.pass));
        }

        for (const auto &cbId: recorderOrder) {
            if (auto *recorder = dynamic_cast<CommandBufferRecorder *>(builtResources.at(cbId))) {
                auto it = recorderToPasses.find(cbId);
                if (it != recorderToPasses.end() && !it->second.empty()) {
                    frame->addPass(recorder, std::move(it->second));
                }
            }
        }
    }
} // Metal
