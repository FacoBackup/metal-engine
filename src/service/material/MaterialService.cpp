#include "MaterialService.h"

#include "../../enum/engine-definitions.h"
#include "MaterialData.h"

#include "../../context/vulkan/VulkanContext.h"
#include "../../util/FilesUtil.h"

#include <cereal/archives/binary.hpp>
#include <fstream>

#include "MaterialInstance.h"
#include "../../context/ApplicationContext.h"
#include "../../enum/LevelOfDetail.h"
#include "../../repository/world/components/MeshComponent.h"
#include "../texture/TextureInstance.h"
#include "../../service/descriptor/DescriptorInstance.h"
#include "../../service/framebuffer/FrameBufferInstance.h"

namespace Metal {
    bool MaterialService::streamAndWrite(std::string &id,
                                         MaterialInstance *instance,
                                         std::unique_ptr<DescriptorInstance> &descriptor) const {
        auto *texture = context.textureService.create(id, LevelOfDetail::LOD_0);
        if (texture == nullptr) {
            return false;
        }
        auto &ref = descriptor->bindings.emplace_back();
        ref.bindingPoint = 0;
        ref.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        ref.sampler = context.coreFrameBuffers.gBufferFBO->vkImageSampler;
        ref.view = texture->vkImageView;
        instance->textures.push_back(id);
        return true;
    }

    MaterialInstance *MaterialService::create(const std::string &id) {
        MaterialData *data = stream(id);
        if (data == nullptr) {
            return nullptr;
        }

        auto *instance = new MaterialInstance(id);
        registerResource(instance);
        instance->descriptorAlbedoTexture = std::make_unique<DescriptorInstance>();
        instance->descriptorNormalTexture = std::make_unique<DescriptorInstance>();
        instance->descriptorRoughnessTexture = std::make_unique<DescriptorInstance>();
        instance->descriptorMetallicTexture = std::make_unique<DescriptorInstance>();
        instance->descriptorHeightTexture = std::make_unique<DescriptorInstance>();

        instance->descriptorAlbedoTexture->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            0));
        instance->descriptorNormalTexture->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            0));
        instance->descriptorRoughnessTexture->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            0));
        instance->descriptorMetallicTexture->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            0));
        instance->descriptorHeightTexture->addLayoutBinding(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            0));

        instance->descriptorAlbedoTexture->create(vulkanContext);
        instance->descriptorNormalTexture->create(vulkanContext);
        instance->descriptorRoughnessTexture->create(vulkanContext);
        instance->descriptorMetallicTexture->create(vulkanContext);
        instance->descriptorHeightTexture->create(vulkanContext);

        instance->useAlbedoTexture = streamAndWrite(data->albedo, instance, instance->descriptorAlbedoTexture);
        instance->useNormalTexture = streamAndWrite(data->normal, instance, instance->descriptorNormalTexture);
        instance->useRoughnessTexture = streamAndWrite(data->roughness, instance, instance->descriptorRoughnessTexture);
        instance->useMetallicTexture = streamAndWrite(data->metallic, instance, instance->descriptorMetallicTexture);
        instance->useHeightTexture = streamAndWrite(data->height, instance, instance->descriptorHeightTexture);

        DescriptorInstance::Write(vulkanContext, instance->descriptorAlbedoTexture->vkDescriptorSet,
                                  instance->descriptorAlbedoTexture->bindings);
        DescriptorInstance::Write(vulkanContext, instance->descriptorNormalTexture->vkDescriptorSet,
                                  instance->descriptorNormalTexture->bindings);
        DescriptorInstance::Write(vulkanContext, instance->descriptorRoughnessTexture->vkDescriptorSet,
                                  instance->descriptorRoughnessTexture->bindings);
        DescriptorInstance::Write(vulkanContext, instance->descriptorMetallicTexture->vkDescriptorSet,
                                  instance->descriptorMetallicTexture->bindings);
        DescriptorInstance::Write(vulkanContext, instance->descriptorHeightTexture->vkDescriptorSet,
                                  instance->descriptorHeightTexture->bindings);

        delete data;

        return instance;
    }

    MaterialData *MaterialService::stream(const std::string &id) const {
        auto pathToFile = context.getAssetDirectory() + FORMAT_FILE_MATERIAL(id);
        if (std::filesystem::exists(pathToFile)) {
            MaterialData *data = new MaterialData;
            PARSE_TEMPLATE(data->load, pathToFile);
            return data;
        }
        return nullptr;
    }
} // Metal
