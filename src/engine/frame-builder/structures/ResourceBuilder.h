#ifndef METAL_ENGINE_RESOURCEBUILDER_H
#define METAL_ENGINE_RESOURCEBUILDER_H
#include <string>
#include "../../enum/ResourceType.h"
#include "../../../common/IContextMember.h"
#include "../../render-graph/RGResourceHandle.h"

namespace Metal {
    class RuntimeResource;

    class ResourceBuilder : public IContextMember {
    protected:
        std::string id;
        RGResourceHandle handle;

    public:
        explicit ResourceBuilder(const std::string &id) : id(id) {
        }

        [[nodiscard]] std::string getId() const {
            return id;
        }

        void setHandle(RGResourceHandle h) {
            handle = h;
        }

        [[nodiscard]] RGResourceHandle getHandle() const {
            return handle;
        }

        virtual ~ResourceBuilder() = default;

        virtual RuntimeResource *build() = 0;

        virtual ResourceType getType() = 0;
    };
}
#endif //METAL_ENGINE_RESOURCEBUILDER_H
