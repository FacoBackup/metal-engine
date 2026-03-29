#ifndef RGPASSBUILDER_H
#define RGPASSBUILDER_H

#include <vector>
#include "RGResourceHandle.h"
#include <unordered_map>

namespace Metal {
    /**
     * @struct RGPassResourceUsage
     * @brief Declares how a resource is used within a specific pass.
     */
    struct RGPassResourceUsage {
        RGResourceHandle handle;
        VkImageLayout layout;
        VkAccessFlags access;
        VkPipelineStageFlags stage;
        bool isExternal = false;
        bool isWrite = false;
    };

    /**
     * @class RGPassBuilder
     * @brief Used during the declaration phase to resolve resource handles and declare usage.
     */
    class RGPassBuilder {
        const std::unordered_map<std::string, RGResourceHandle>& resourceHandles;
        std::vector<RGPassResourceUsage> usages;

    public:
        explicit RGPassBuilder(const std::unordered_map<std::string, RGResourceHandle>& resourceHandles) 
            : resourceHandles(resourceHandles) {}

        [[nodiscard]] RGResourceHandle getResourceHandle(const std::string& id) const {
            auto it = resourceHandles.find(id);
            if (it != resourceHandles.end()) {
                return it->second;
            }
            return RGResourceHandle();
        }

        void read(RGResourceHandle handle, VkImageLayout layout, VkAccessFlags access, VkPipelineStageFlags stage) {
            usages.push_back({handle, layout, access, stage, false, false});
        }

        void write(RGResourceHandle handle, VkImageLayout layout, VkAccessFlags access, VkPipelineStageFlags stage) {
            usages.push_back({handle, layout, access, stage, false, true});
        }

        void readExternal(RGResourceHandle handle, VkImageLayout layout, VkAccessFlags access, VkPipelineStageFlags stage) {
            usages.push_back({handle, layout, access, stage, true, false});
        }

        void clear() {
            usages.clear();
        }

        [[nodiscard]] const std::vector<RGPassResourceUsage>& getUsages() const {
            return usages;
        }
    };
}

#endif // RGPASSBUILDER_H
