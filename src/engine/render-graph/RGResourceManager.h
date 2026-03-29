#ifndef RGRESOURCEMANAGER_H
#define RGRESOURCEMANAGER_H

#include "RGResourceHandle.h"
#include "../resource/RuntimeResource.h"
#include <vector>

namespace Metal {
    /**
     * @class RGResourceManager
     * @brief Provides access to physical resources during the execution phase of the Render Graph.
     */
    class RGResourceManager {
        const std::vector<RuntimeResource*>& resources;

    public:
        explicit RGResourceManager(const std::vector<RuntimeResource*>& resources) : resources(resources) {}

        template<typename T>
        T* get(RGResourceHandle handle) const {
            if (handle.isValid() && handle.getIndex() < resources.size()) {
                return dynamic_cast<T*>(resources[handle.getIndex()]);
            }
            return nullptr;
        }
    };
}

#endif // RGRESOURCEMANAGER_H
