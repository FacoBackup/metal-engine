#include "EngineFrame.h"
#include "../passes/CommandBufferRecorder.h"
#include "../passes/AbstractPass.h"
#include "../render-graph/RGResourceManager.h"

namespace Metal {
    EngineFrame::EngineFrame(std::string id) : id(std::move(id)) {
    }

    void EngineFrame::addResource(RuntimeResource *resource) {
        if (resource) {
            resources[resource->getId()] = resource;
        }
    }

    void EngineFrame::addResource(RGResourceHandle handle, RuntimeResource *resource) {
        if (resource) {
            resources[resource->getId()] = resource;
            if (handle.isValid()) {
                if (handle.getIndex() >= indexedResources.size()) {
                    indexedResources.resize(handle.getIndex() + 1, nullptr);
                }
                indexedResources[handle.getIndex()] = resource;
            }
        }
    }

    void EngineFrame::addPass(CommandBufferRecorder *recorder, std::vector<std::unique_ptr<AbstractPass>> p) {
        if (recorder) {
            passes.emplace_back(recorder, std::move(p));
        }
    }

    void EngineFrame::onSync() {
        RGResourceManager resourceManager(indexedResources);
        for (auto &pair: passes) {
            pair.first->recordCommands(pair.second, resourceManager);
        }
    }

    std::string EngineFrame::getScopedResourceId(const std::string &resourceId) const {
        return id + "_" + resourceId;
    }
}
