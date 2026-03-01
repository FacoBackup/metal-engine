#ifndef METAL_ENGINE_ENGINEFRAME_H
#define METAL_ENGINE_ENGINEFRAME_H

#include <vector>
#include <string>
#include <unordered_map>
#include "../../../repository/abstract/RuntimeResource.h"
#include "../passes/CommandBufferRecorder.h"
#include "../passes/AbstractPass.h"

namespace Metal {
    class CommandBufferRecorder;
    class AbstractPass;
    class EngineFrame {
        std::unordered_map<std::string, RuntimeResource *> resources;
        std::vector<std::pair<CommandBufferRecorder *, std::vector<AbstractPass *> > > passes;

    public:
        void addResource(RuntimeResource *resource) {
            if (resource) {
                resources[resource->getId()] = resource;
            }
        }

        void addPass(CommandBufferRecorder *recorder, const std::vector<AbstractPass *> &p) {
            if (recorder) {
                passes.push_back({recorder, p});
            }
        }

        [[nodiscard]] const std::vector<std::pair<CommandBufferRecorder *, std::vector<AbstractPass *> > > &getPasses() const {
            return passes;
        }

        void onSync() {
            for (auto &pair: passes) {
                pair.first->recordCommands(pair.second);
            }
        }

        void dispose() {
            for (auto &pair: passes) {
                delete pair.first;
                for (auto *pass: pair.second) {
                    delete pass;
                }
            }
            passes.clear();
        }

        RuntimeResource *getResource(const std::string &id) {
            if (resources.contains(id)) {
                return resources[id];
            }
            return nullptr;
        }

        void destroy() {
            // for (auto &pair: resources) {
            //     pair.second->dispose();
            // }
            // resources.clear();
        }
    };
}

#endif //METAL_ENGINE_ENGINEFRAME_H
