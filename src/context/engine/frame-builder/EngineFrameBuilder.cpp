#include "EngineFrameBuilder.h"

#include "structures/FramebufferBuilder.h"
#include "structures/TextureBuilder.h"
#include "structures/BufferBuilder.h"
#include "structures/PassBuilder.h"
#include "structures/CommandBufferRecorderBuilder.h"
#include "EngineFrame.h"
#include "../../../enum/EngineResourceIDs.h"
#include "../../ApplicationContext.h"
#include "../passes/CommandBufferRecorder.h"
#include "../render-pass/impl/PostProcessingPass.h"
#include "../compute-pass/impl/HWRayTracingPass.h"
#include "../compute-pass/impl/AccumulationPass.h"
#include "../render-pass/impl/tools/SelectedDotPass.h"
#include "../render-pass/impl/tools/GridPass.h"
#include "../render-pass/impl/tools/IconsPass.h"

namespace Metal {
    EngineFrameBuilder::EngineFrameBuilder(std::string frameId) : frameId(std::move(frameId)) {
    }

    EngineFrameBuilder &EngineFrameBuilder::addFramebuffer(std::string id, const unsigned w, const unsigned h,
                                                           glm::vec4 clearColor) {
        currentBuilder = std::make_shared<FramebufferBuilder>(frameId + "_" + id, w, h, clearColor);
        builders.push_back(currentBuilder);
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addFramebuffer(const std::string &id) {
        if (!tryMatch(frameId + "_" + id, ResourceType::FRAMEBUFFER)) {
            throw std::runtime_error("Framebuffer not found");
        }
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addColor(std::string id, VkFormat format, VkImageUsageFlagBits usage,
                                                     FrameBufferInstance *framebuffer) {
        dynamic_cast<FramebufferBuilder *>(currentBuilder.get())->addColor(frameId + "_" + id, format, usage, framebuffer);
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addDepth() {
        dynamic_cast<FramebufferBuilder *>(currentBuilder.get())->addDepth();
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addTexture(const std::string &id, unsigned w, unsigned h, VkFormat format) {
        currentBuilder = std::make_shared<TextureBuilder>(frameId + "_" + id, w, h, format);
        builders.push_back(currentBuilder);
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
            frameId + "_" + id, size, type == UNIFORM_BUFFER ? VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT : VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            properties, type);
        builders.push_back(currentBuilder);
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addBuffer(const std::string &id) {
        if (!tryMatch(frameId + "_" + id, ResourceType::BUFFER)) {
            throw std::runtime_error("Buffer not found");
        }
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addCommandBuffer(const std::string &id, const std::string &framebufferId,
                                                            const bool clearBuffer) {
        currentBuilder = std::make_shared<CommandBufferRecorderBuilder>(frameId + "_" + id, frameId + "_" + framebufferId, clearBuffer);
        builders.push_back(currentBuilder);
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addComputeCommandBuffer(const std::string &id) {
        currentBuilder = std::make_shared<CommandBufferRecorderBuilder>(frameId + "_" + id);
        builders.push_back(currentBuilder);
        return *this;
    }

    EngineFrameBuilder &EngineFrameBuilder::addPass(PassType type, const std::string &commandBufferId) {
        currentBuilder = std::make_shared<PassBuilder>(Util::uuidV4(), type, frameId + "_" + commandBufferId);
        builders.push_back(currentBuilder);
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

    std::unique_ptr<EngineFrame> EngineFrameBuilder::build() {
        auto frame = std::make_unique<EngineFrame>(frameId);
        std::unordered_map<std::string, RuntimeResource *> builtResources;

        for (const auto &builder: builders) {
            auto *resource = builder->build();
            if (resource) {
                frame->addResource(resource);
                builtResources[builder->getId()] = resource;
            }
        }

        std::unordered_map<std::string, std::vector<AbstractPass *> > recorderToPasses;
        std::vector<std::string> recorderOrder;

        for (const auto &builder: builders) {
            if (auto *passBuilder = dynamic_cast<PassBuilder *>(builder.get())) {
                const auto cbId = passBuilder->getCommandBufferId();
                if (std::find(recorderOrder.begin(), recorderOrder.end(), cbId) == recorderOrder.end()) {
                    recorderOrder.push_back(cbId);
                }

                if (builtResources.contains(builder->getId())) {
                    auto *pass = dynamic_cast<AbstractPass *>(builtResources.at(builder->getId()));
                    if (pass) {
                        pass->frame = frame.get();
                        dynamic_cast<Initializable *>(pass)->onInitialize();
                        recorderToPasses[cbId].push_back(pass);
                    }
                }
            }
        }

        for (const auto &cbId: recorderOrder) {
            auto *recorder = CTX.commandBufferRecorderService.getResource(cbId);
            if (recorder) {
                frame->addPass(recorder, recorderToPasses[cbId]);
            }
        }

        return frame;
    }
} // Metal
