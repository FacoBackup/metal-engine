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

#include "../render-graph/RGPassBuilder.h"
#include "../render-graph/RGResourceState.h"
#include "../render-graph/RGBarrierBatch.h"
#include "../render-graph/RGResourceAllocator.h"
#include "../resource/TextureInstance.h"
#include "../resource/RenderTargetInstance.h"
#include "../resource/BufferInstance.h"

#include <algorithm>
#include <queue>
#include <map>
#include <set>

namespace Metal {
    EngineFrameBuilder::EngineFrameBuilder(std::string frameId) : frameId(std::move(frameId)) {
    }

    EngineFrameBuilder& EngineFrameBuilder::addRenderTarget(std::string id, const unsigned w, const unsigned h,
                                                           glm::vec4 clearColor, RGResourceHandle* outHandle) {
        currentBuilder = std::make_shared<RenderTargetBuilder>(frameId + "_" + id, w, h, clearColor);
        storeBuilder();
        if (outHandle) *outHandle = currentBuilder->getHandle();
        return *this;
    }

    EngineFrameBuilder& EngineFrameBuilder::addRenderTarget(const std::string &id, RGResourceHandle* outHandle) {
        if (!tryMatch(frameId + "_" + id, ResourceType::RENDER_TARGET)) {
            throw std::runtime_error("RenderTarget not found");
        }
        if (outHandle) *outHandle = currentBuilder->getHandle();
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

    EngineFrameBuilder& EngineFrameBuilder::addTexture(const std::string &id, unsigned w, unsigned h, VkFormat format, RGResourceHandle* outHandle) {
        currentBuilder = std::make_shared<TextureBuilder>(frameId + "_" + id, w, h, format);
        storeBuilder();
        if (outHandle) *outHandle = currentBuilder->getHandle();
        return *this;
    }

    EngineFrameBuilder& EngineFrameBuilder::addTexture(const std::string &id, RGResourceHandle* outHandle) {
        if (!tryMatch(frameId + "_" + id, ResourceType::TEXTURE)) {
            throw std::runtime_error("Texture not found");
        }
        if (outHandle) *outHandle = currentBuilder->getHandle();
        return *this;
    }

    EngineFrameBuilder& EngineFrameBuilder::addBuffer(const std::string &id, VkDeviceSize size,
                                                      VkMemoryPropertyFlags properties, BufferType type, RGResourceHandle* outHandle) {
        currentBuilder = std::make_shared<BufferBuilder>(
            frameId + "_" + id, size, type == UNIFORM_BUFFER
                                          ? VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
                                          : VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            properties, type);
        storeBuilder();
        if (outHandle) *outHandle = currentBuilder->getHandle();
        return *this;
    }

    EngineFrameBuilder& EngineFrameBuilder::addBuffer(const std::string &id, RGResourceHandle* outHandle) {
        if (!tryMatch(frameId + "_" + id, ResourceType::BUFFER)) {
            throw std::runtime_error("Buffer not found");
        }
        if (outHandle) *outHandle = currentBuilder->getHandle();
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
        currentBuilder->setHandle(RGResourceHandle(static_cast<uint32_t>(builders.size())));
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

        std::unordered_map<std::string, RGResourceHandle> resourceHandles;
        for (const auto &builder: builders) {
            resourceHandles[builder->getId()] = builder->getHandle();
        }

        RGResourceAllocator allocator;
        ctx->injectDependencies(&allocator);

        for (const auto &builder: builders) {
            if (auto *resource = builder->build()) {
                frame->addResource(builder->getHandle(), resource);
                builtResources[builder->getId()] = resource;
                builtResources[static_cast<std::string>(builder->getHandle())] = resource;
            }
        }

        std::unordered_map<std::string, std::vector<std::unique_ptr<AbstractPass> > > recorderToPasses;
        std::vector<std::string> recorderOrder;

        RGPassBuilder rgPassBuilder(resourceHandles);
        struct PassUsageInfo {
            AbstractPass* pass;
            std::vector<RGPassResourceUsage> usages;
            std::string commandBufferId;
            uint32_t originalIndex;
        };
        std::vector<PassUsageInfo> allPasses;

        for (uint32_t i = 0; i < passes.size(); ++i) {
            auto& pass = passes[i];
            pass.pass->frame = frame;
            ctx->injectDependencies(pass.pass.get());

            for (const auto &builder: builders) {
                pass.pass->addResourceHandle(builder->getId(), builder->getHandle());
            }

            rgPassBuilder.clear();
            pass.pass->setup(rgPassBuilder);
            
            allPasses.push_back({pass.pass.get(), rgPassBuilder.getUsages(), pass.commandBufferId, i});
        }

        // --- DEPENDENCY RESOLUTION (Topological Sort) ---
        const uint32_t passCount = static_cast<uint32_t>(allPasses.size());
        std::vector<std::vector<uint32_t>> adj(passCount);
        std::vector<int> inDegree(passCount, 0);

        for (uint32_t i = 0; i < passCount; ++i) {
            for (uint32_t j = 0; j < passCount; ++j) {
                if (i == j) continue;
                
                bool depends = false;
                for (const auto& usageI : allPasses[i].usages) {
                    if (usageI.isExternal) continue;
                    if (!usageI.isWrite) continue;

                    for (const auto& usageJ : allPasses[j].usages) {
                        if (usageJ.isExternal) continue;
                        if (usageI.handle.getIndex() == usageJ.handle.getIndex() && !usageJ.isWrite) {
                            depends = true;
                            break;
                        }
                    }
                    if (depends) break;
                }

                if (depends) {
                    adj[i].push_back(static_cast<uint32_t>(j));
                    inDegree[j]++;
                }
            }
        }

        std::vector<uint32_t> sortedIndices;
        std::queue<uint32_t> q;
        for (uint32_t i = 0; i < allPasses.size(); ++i) {
            if (inDegree[i] == 0) q.push(i);
        }

        while (!q.empty()) {
            uint32_t u = q.front();
            q.pop();
            sortedIndices.push_back(u);

            for (uint32_t v : adj[u]) {
                inDegree[v]--;
                if (inDegree[v] == 0) q.push(v);
            }
        }

        if (sortedIndices.size() != allPasses.size()) {
            // Fallback to original order if cycle detected (should not happen in standard RG)
            sortedIndices.clear();
            for (uint32_t i = 0; i < allPasses.size(); ++i) sortedIndices.push_back(i);
        }

        std::vector<PassUsageInfo> passExecutionOrder;
        for (uint32_t idx : sortedIndices) {
            passExecutionOrder.push_back(allPasses[idx]);
        }

        for (auto& info : passExecutionOrder) {
            info.pass->onInitialize();
            recorderToPasses[info.commandBufferId].push_back(std::unique_ptr<AbstractPass>(info.pass));
        }
        
        // Remove from passes vector since they are now owned by recorderToPasses
        // Wait, info.pass is a raw pointer to pass originally in 'passes' vector
        // We need to be careful with ownership.
        // Actually, we should move from 'passes' to 'recorderToPasses' based on sortedIndices.
        
        // Re-calculate recorderToPasses with correct order
        recorderToPasses.clear();
        recorderOrder.clear();
        std::set<std::string> addedRecorders;
        
        for (uint32_t idx : sortedIndices) {
            auto& passData = passes[idx];
            if (addedRecorders.find(passData.commandBufferId) == addedRecorders.end()) {
                recorderOrder.push_back(passData.commandBufferId);
                addedRecorders.insert(passData.commandBufferId);
            }
            // We'll move them later or handle it carefully.
        }
        
        // Re-filling recorderToPasses using unique_ptrs from 'passes'
        // But 'passes' is a vector of PassData, we need to move out of it in sorted order.
        std::vector<std::unique_ptr<AbstractPass>> sortedPasses(passes.size());
        for (uint32_t i = 0; i < sortedIndices.size(); ++i) {
            uint32_t originalIdx = sortedIndices[i];
            recorderToPasses[passes[originalIdx].commandBufferId].push_back(std::move(passes[originalIdx].pass));
        }

        // --- LIVENESS ANALYSIS ---
        std::map<uint32_t, LivenessInterval> liveness;
        for (uint32_t i = 0; i < passExecutionOrder.size(); ++i) {
            for (const auto& usage : passExecutionOrder[i].usages) {
                uint32_t resIdx = usage.handle.getIndex();
                liveness[resIdx].start = std::min(liveness[resIdx].start, i);
                liveness[resIdx].end = std::max(liveness[resIdx].end, i);
            }
        }

        // --- BARRIER GENERATION ---
        std::unordered_map<uint32_t, RGResourceState> globalState;

        for (auto& info : passExecutionOrder) {
            RGBarrierBatch batch;
            for (const auto& usage : info.usages) {
                RGResourceState& currentState = globalState[usage.handle.getIndex()];
                
                if (currentState.layout != usage.layout || 
                    (currentState.access & usage.access) != usage.access) {
                    
                    auto* resource = frame->getResourceAs<RuntimeResource>(usage.handle);
                    if (!resource) continue;

                    if (resource->resourceType() == TEXTURE || resource->resourceType() == RENDER_TARGET) {
                        VkImage image = VK_NULL_HANDLE;
                        if (resource->resourceType() == TEXTURE) {
                            image = dynamic_cast<TextureInstance*>(resource)->vkImage;
                        } else {
                            // Simplified: use first attachment for RT barrier
                            auto* rt = dynamic_cast<RenderTargetInstance*>(resource);
                            if (!rt->attachments.empty()) {
                                image = rt->attachments[0]->vkImage;
                            }
                        }

                        if (image != VK_NULL_HANDLE) {
                            VkImageMemoryBarrier barrier{};
                            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                            barrier.oldLayout = currentState.layout;
                            barrier.newLayout = usage.layout;
                            barrier.srcAccessMask = currentState.access;
                            barrier.dstAccessMask = usage.access;
                            barrier.image = image;
                            barrier.subresourceRange.aspectMask = (usage.layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) 
                                ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
                            barrier.subresourceRange.baseMipLevel = 0;
                            barrier.subresourceRange.levelCount = 1;
                            barrier.subresourceRange.baseArrayLayer = 0;
                            barrier.subresourceRange.layerCount = 1;

                            batch.addImageBarrier(barrier, currentState.stage, usage.stage);
                        }
                    } else if (resource->resourceType() == BUFFER) {
                        auto* buffer = dynamic_cast<BufferInstance*>(resource);
                        VkBufferMemoryBarrier barrier{};
                        barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
                        barrier.srcAccessMask = currentState.access;
                        barrier.dstAccessMask = usage.access;
                        barrier.buffer = buffer->vkBuffer;
                        barrier.offset = 0;
                        barrier.size = buffer->dataSize;

                        batch.addBufferBarrier(barrier, currentState.stage, usage.stage);
                    }

                    currentState.layout = usage.layout;
                    currentState.access = usage.access;
                    currentState.stage = usage.stage;
                }
            }
            info.pass->setBarrierBatch(batch);
        }

        for (const auto &cbId: recorderOrder) {
            auto itBuilt = builtResources.find(cbId);
            if (itBuilt != builtResources.end()) {
                if (auto *recorder = dynamic_cast<CommandBufferRecorder *>(itBuilt->second)) {
                    auto it = recorderToPasses.find(cbId);
                    if (it != recorderToPasses.end() && !it->second.empty()) {
                        frame->addPass(recorder, std::move(it->second));
                    }
                }
            }
        }
    }
} // Metal
