#ifndef METAL_ENGINE_RESOURCEBUILDER_H
#define METAL_ENGINE_RESOURCEBUILDER_H
#include <string>
#include "../../enum/ResourceType.h"

namespace Metal {
    class RuntimeResource;
    class ResourceBuilder {
    protected:
        std::string id;

    public:
        explicit ResourceBuilder(const std::string &id) : id(id) {
        }

        [[nodiscard]] std::string getId() const {
            return id;
        }
        virtual ~ResourceBuilder() = default;

        virtual RuntimeResource* build() = 0;

        virtual ResourceType getType() = 0;
    };
}
#endif //METAL_ENGINE_RESOURCEBUILDER_H
