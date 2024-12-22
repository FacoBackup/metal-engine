#ifndef IGPURESOURCE_H
#define IGPURESOURCE_H
#include "ResourceType.h"
#include "../../common/util/Util.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    class VulkanContext;

    class RuntimeResource {
        const std::string id;
        bool noDisposal = false;

    public:
        virtual ~RuntimeResource() = default;

        explicit RuntimeResource(const std::string &id = Util::uuidV4()): id(std::move(id)) {
        }

        long long lastUse = 0;

        [[nodiscard]] std::string getId() const {
            return id;
        }

        virtual void dispose(VulkanContext &context) {
            throw std::runtime_error("Not implemented");
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
