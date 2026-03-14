#ifndef METAL_ENGINE_ENGINEFRAME_H
#define METAL_ENGINE_ENGINEFRAME_H

#include <vector>
#include <string>
#include <unordered_map>
#include "../resource/RuntimeResource.h"
#include "../passes/CommandBufferRecorder.h"
#include "../passes/AbstractPass.h"

namespace Metal {
    class CommandBufferRecorder;
    class AbstractPass;

    class EngineFrame {
        std::string id;
        bool shouldRender = false;
        std::unordered_map<std::string, RuntimeResource *> resources;
        std::vector<std::pair<CommandBufferRecorder *, std::vector<std::unique_ptr<AbstractPass> > > > passes;

    public:
        explicit EngineFrame(std::string id);

        [[nodiscard]] const std::string &getId() const { return id; }

        void setShouldRender(const bool val) { shouldRender = val; }

        [[nodiscard]] bool getShouldRender() const { return shouldRender; }

        void addResource(RuntimeResource *resource);

        template<typename T>
        T *getResourceAs(const std::string &resourceId) {
            auto it = resources.find(getScopedResourceId(resourceId));
            if (it != resources.end()) {
                return dynamic_cast<T *>(it->second);
            }
            return nullptr;
        }

        void addPass(CommandBufferRecorder *recorder, const std::vector<std::unique_ptr<AbstractPass>> &p);

        void onSync();

        [[nodiscard]] std::string getScopedResourceId(const std::string &resourceId) const;
    };
}

#endif //METAL_ENGINE_ENGINEFRAME_H
