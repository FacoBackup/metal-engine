#ifndef IGPURESOURCE_H
#define IGPURESOURCE_H
#include "../enum/ResourceType.h"
#include "../../editor/util/Util.h"
#include <chrono>

#include "../../common/IContextMember.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    class VulkanContext;

    class RuntimeResource : public IContextMember {
        const std::string id;
        bool noDisposal = false;

    protected:
        VulkanContext *vulkanContext = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {{"VulkanContext", &vulkanContext}};
        }

        virtual ~RuntimeResource() = default;

        explicit RuntimeResource(const std::string &id = Util::uuidV4()) : id(id) {
        }

        [[nodiscard]] std::string getId() const {
            return id;
        }

        void setAsNoDisposal() {
            noDisposal = true;
        }

        bool isNoDisposal() const {
            return noDisposal;
        }

        virtual ResourceType resourceType() {
            throw std::runtime_error("Not implemented");
        }
    };
}
#endif
