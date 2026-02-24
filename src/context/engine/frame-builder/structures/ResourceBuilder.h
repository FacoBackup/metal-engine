#ifndef METAL_ENGINE_RESOURCEBUILDER_H
#define METAL_ENGINE_RESOURCEBUILDER_H
#include <string>

namespace Metal {
    class ResourceBuilder {
        std::string id;

    public:
        explicit ResourceBuilder(const std::string &id) : id(std::move(id)) {
        }
    };
}
#endif //METAL_ENGINE_RESOURCEBUILDER_H
