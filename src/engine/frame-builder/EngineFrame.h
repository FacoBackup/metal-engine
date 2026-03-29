#ifndef METAL_ENGINE_ENGINEFRAME_H
#define METAL_ENGINE_ENGINEFRAME_H

#include <vector>
#include <string>
#include <unordered_map>
#include "../resource/RuntimeResource.h"
#include "../passes/CommandBufferRecorder.h"
#include "../passes/AbstractPass.h"

#include "../render-graph/RGResourceHandle.h"

namespace Metal {
    class CommandBufferRecorder;
    class AbstractPass;

    class EngineFrame {
        std::string id;
        bool shouldRender = false;
        std::unordered_map<std::string, RuntimeResource *> resources;
        std::vector<RuntimeResource *> indexedResources;
        std::vector<std::pair<CommandBufferRecorder *, std::vector<std::unique_ptr<AbstractPass> > > > passes;

    public:
        explicit EngineFrame(std::string id);

        [[nodiscard]] const std::string &getId() const { return id; }

        void setShouldRender(const bool val) { shouldRender = val; }

        [[nodiscard]] bool getShouldRender() const { return shouldRender; }

        template<typename T>
        T *getResourceAs(const std::string &resourceId) {
            auto it = resources.find(getScopedResourceId(resourceId));
            if (it != resources.end()) {
                return dynamic_cast<T *>(it->second);
            }
            return nullptr;
        }

        template<typename T>
        T *getResourceAs(RGResourceHandle handle) {
            if (handle.isValid() && handle.getIndex() < indexedResources.size()) {
                return dynamic_cast<T *>(indexedResources[handle.getIndex()]);
            }
            return nullptr;
        }

        void addResource(RuntimeResource *resource);

        void addResource(RGResourceHandle handle, RuntimeResource *resource);

        void addPass(CommandBufferRecorder *recorder, std::vector<std::unique_ptr<AbstractPass>> p);

        void onSync();

        [[nodiscard]] std::string getScopedResourceId(const std::string &resourceId) const;
    };
}

#endif //METAL_ENGINE_ENGINEFRAME_H
