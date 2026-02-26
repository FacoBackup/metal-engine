#ifndef METAL_ENGINE_RESOURCEBUILDER_H
#define METAL_ENGINE_RESOURCEBUILDER_H
#include <string>

namespace Metal {
    class RuntimeResource;
    enum ResourceBuilderType {
        TEXTURE,
        FRAMEBUFFER,
    };
    class ResourceBuilder {
    protected:
        std::string id;

    public:
        explicit ResourceBuilder(const std::string &id) : id(std::move(id)) {
        }

        [[nodiscard]] std::string getId() const {
            return id;
        }
        virtual ~ResourceBuilder() = default;

        virtual RuntimeResource* build() = 0;

        virtual ResourceBuilderType getType() = 0;
    };
}
#endif //METAL_ENGINE_RESOURCEBUILDER_H
