#ifndef METAL_ENGINE_ENGINEFRAME_H
#define METAL_ENGINE_ENGINEFRAME_H

#include <string>
#include <unordered_map>
#include "../../../repository/abstract/RuntimeResource.h"

namespace Metal {
    class EngineFrame {
        std::unordered_map<std::string, RuntimeResource *> resources;

    public:
        void addResource(RuntimeResource *resource) {
            if (resource) {
                resources[resource->getId()] = resource;
            }
        }

        RuntimeResource *getResource(const std::string &id) {
            if (resources.contains(id)) {
                return resources[id];
            }
            return nullptr;
        }

        template<typename T>
        T *getResourceAs(const std::string &id) {
            return static_cast<T *>(getResource(id));
        }

        void destroy() {
            for (auto &pair: resources) {
                pair.second->dispose();
            }
            resources.clear();
        }
    };
}

#endif //METAL_ENGINE_ENGINEFRAME_H
