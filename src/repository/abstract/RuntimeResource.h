#ifndef IGPURESOURCE_H
#define IGPURESOURCE_H
#include "../../enum/ResourceType.h"
#include "../../util/Util.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

namespace Metal {
    class VulkanContext;

    class RuntimeResource {
        const std::string id;
        bool noDisposal = false;
        std::vector<std::string> empty{};
    public:
        virtual ~RuntimeResource() = default;

        explicit RuntimeResource(const std::string &id = Util::uuidV4()): id(std::move(id)) {
        }

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

        virtual std::vector<std::string> &getDependencies() {
            return empty;
        }
    };
}
#endif
