#ifndef MATERIALINSTANCE_H
#define MATERIALINSTANCE_H
#include <glm/vec3.hpp>
#include <string>
#include <vector>

#include "../../repository/abstract/RuntimeResource.h"
#include "../../service/descriptor/DescriptorInstance.h"

namespace Metal {
    struct MaterialInstance final : RuntimeResource {
        explicit MaterialInstance(const std::string &id) : RuntimeResource(id) {
        }

        std::vector<std::string> textures{};
        unsigned int materialIndex = 0;

        std::vector<std::string> &getDependencies() override {
            return textures;
        }
    };
} // Metal

#endif //MATERIALINSTANCE_H
