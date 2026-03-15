#include "EngineFrame.h"
#include "../passes/CommandBufferRecorder.h"
#include "../passes/AbstractPass.h"

namespace Metal {
    EngineFrame::EngineFrame(std::string id) : id(std::move(id)) {
    }

    void EngineFrame::addResource(RuntimeResource *resource) {
        if (resource) {
            resources[resource->getId()] = resource;
        }
    }

    void EngineFrame::addPass(CommandBufferRecorder *recorder, std::vector<std::unique_ptr<AbstractPass>> p) {
        if (recorder) {
            passes.emplace_back(recorder, std::move(p));
        }
    }

    void EngineFrame::onSync() {
        for (auto &pair: passes) {
            pair.first->recordCommands(pair.second);
        }
    }

    std::string EngineFrame::getScopedResourceId(const std::string &resourceId) const {
        return id + "_" + resourceId;
    }
}
