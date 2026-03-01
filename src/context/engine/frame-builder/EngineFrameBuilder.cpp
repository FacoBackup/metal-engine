#include "EngineFrameBuilder.h"

#include "structures/FramebufferBuilder.h"
#include "structures/TextureBuilder.h"
#include "structures/BufferBuilder.h"
#include "EngineFrame.h"
#include "../../../enum/EngineResourceIDs.h"
#include "../../ApplicationContext.h"
#include "../passes/CommandBufferRecorder.h"
#include "../render-pass/impl/GBufferGenPass.h"
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

    EngineFrameBuilder &EngineFrameBuilder::addTexture(const std::string &id, unsigned w, unsigned h) {
        currentBuilder = std::make_shared<TextureBuilder>(frameId + "_" + id, w, h);
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

    EngineFrameBuilder &EngineFrameBuilder::addPass(PassType type) {
        passTypes.push_back(type);
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
        for (const auto &builder: builders) {
            frame->addResource(builder->build());
        }

        auto &fbService = CTX.framebufferService;
        for (const auto &passType: passTypes) {
            switch (passType) {
                case GBUFFER: {
                    auto *fbo = fbService.getResource(frameId + "_" + RID_G_BUFFER_FBO);
                    auto *pass = new GBufferGenPass();
                    pass->frame = frame.get();
                    pass->onInitialize();
                    auto *recorder = new CommandBufferRecorder(fbo);
                    frame->addPass(recorder, {pass});
                    break;
                }
                case COMPUTE: {
                    auto *pass1 = new HWRayTracingPass();
                    pass1->frame = frame.get();
                    pass1->onInitialize();
                    auto *pass2 = new AccumulationPass();
                    pass2->frame = frame.get();
                    pass2->onInitialize();
                    auto *recorder = new CommandBufferRecorder();
                    frame->addPass(recorder, {pass1, pass2});
                    break;
                }
                case POST_PROCESSING: {
                    auto *fbo = fbService.getResource(frameId + "_" + RID_POST_PROCESSING_FBO);
                    std::vector<AbstractPass *> passes;
                    auto *ppPass = new PostProcessingPass();
                    ppPass->frame = frame.get();
                    ppPass->onInitialize();
                    passes.push_back(ppPass);

                    if (CTX.isDebugMode()) {
                        auto *dot = new SelectedDotPass();
                        dot->frame = frame.get();
                        dot->onInitialize();
                        passes.push_back(dot);

                        auto *grid = new GridPass();
                        grid->frame = frame.get();
                        grid->onInitialize();
                        passes.push_back(grid);

                        auto *icons = new IconsPass();
                        icons->frame = frame.get();
                        icons->onInitialize();
                        passes.push_back(icons);
                    }

                    auto *recorder = new CommandBufferRecorder(fbo);
                    frame->addPass(recorder, passes);
                    break;
                }
                default:
                    break;
            }
        }

        return frame;
    }
} // Metal
