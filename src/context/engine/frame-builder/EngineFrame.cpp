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


    void EngineFrame::addPass(CommandBufferRecorder *recorder, const std::vector<AbstractPass *> &p) {
        if (recorder) {
            passes.emplace_back(recorder, p);
        }
    }

    void EngineFrame::onSync() {
        for (auto &pair: passes) {
            pair.first->recordCommands(pair.second);
        }
    }

    void EngineFrame::dispose() {
        for (auto &pair: passes) {
            delete pair.first;
            for (auto *pass: pair.second) {
                delete pass;
            }
        }
        passes.clear();
    }

    std::string EngineFrame::getScopedResourceId(const std::string &resourceId) const {
        return id + "_" + resourceId;
    }
}
