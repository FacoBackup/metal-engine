#ifndef MATERIALINSTANCE_H
#define MATERIALINSTANCE_H
#include <glm/vec3.hpp>
#include <string>
#include <vector>

#include "../../repository/abstract/RuntimeResource.h"
#include "../../service/descriptor/DescriptorInstance.h"

namespace Metal {
    struct MaterialInstance final : RuntimeResource {
        explicit MaterialInstance(const std::string &id): RuntimeResource(id) {
        }

        std::unique_ptr<DescriptorInstance> descriptorAlbedoTexture = nullptr;
        std::unique_ptr<DescriptorInstance> descriptorNormalTexture = nullptr;
        std::unique_ptr<DescriptorInstance> descriptorRoughnessTexture = nullptr;
        std::unique_ptr<DescriptorInstance> descriptorMetallicTexture = nullptr;
        std::unique_ptr<DescriptorInstance> descriptorAOTexture = nullptr;
        std::unique_ptr<DescriptorInstance> descriptorHeightTexture = nullptr;
        std::vector<std::string> textures{};

        bool useAlbedoTexture{};
        bool useNormalTexture{};
        bool useRoughnessTexture{};
        bool useMetallicTexture{};
        bool useAOTexture{};
        bool useHeightTexture{};

        void dispose(VulkanContext &context) override;

        std::vector<std::string> &getDependencies() override {
            return textures;
        }
    };
} // Metal

#endif //MATERIALINSTANCE_H
