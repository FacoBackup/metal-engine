#include "MaterialService.h"

#include "../../enum/engine-definitions.h"
#include "MaterialFileData.h"

#include "../../context/vulkan/VulkanContext.h"
#include "../../util/FilesUtil.h"
#include "../../util/serialization-definitions.h"

#include <fstream>
#include "../../dto/buffers/MaterialData.h"
#include "../buffer/BufferInstance.h"

#include "MaterialInstance.h"
#include "../../context/ApplicationContext.h"
#include "../../repository/world/components/MeshComponent.h"
#include "../texture/TextureInstance.h"
#include "../../service/descriptor/DescriptorInstance.h"
#include "../../service/framebuffer/FrameBufferInstance.h"

namespace Metal {
    MaterialInstance *MaterialService::create(const std::string &id) {
        MaterialFileData *data = stream(id);
        if (data == nullptr) {
            return nullptr;
        }

        auto *instance = createResourceInstance(id);
        instance->materialIndex = nextMaterialIndex++;
        MaterialData materialData{};
        materialData.albedo = data->albedoColor;
        materialData.roughness = data->roughnessFactor;
        materialData.metallic = data->metallicFactor;
        materialData.isEmissive = data->isEmissive ? 1 : 0;

        materialData.useAlbedoTexture = !data->albedo.empty();
        materialData.useNormalTexture = !data->normal.empty();
        materialData.useRoughnessTexture = !data->roughness.empty();
        materialData.useMetallicTexture = !data->metallic.empty();
        materialData.useAoTexture = !data->ao.empty();
        materialData.useHeightTexture = !data->height.empty();

        materialData.albedoTextureId = 0;
        materialData.normalTextureId = 0;
        materialData.roughnessTextureId = 0;
        materialData.metallicTextureId = 0;
        materialData.aoTextureId = 0;
        materialData.heightTextureId = 0;

        if (!data->albedo.empty()) {
            auto *tex = CTX.textureService.create(data->albedo);
            if (tex != nullptr) {
                materialData.albedoTextureId = CTX.textureService.getTextureIndex(data->albedo);
            }
        }
        if (!data->normal.empty()) {
            auto *tex = CTX.textureService.create(data->normal);
            if (tex != nullptr) {
                materialData.normalTextureId = CTX.textureService.getTextureIndex(data->normal);
            }
        }
        if (!data->roughness.empty()) {
            auto *tex = CTX.textureService.create(data->roughness);
            if (tex != nullptr) {
                materialData.roughnessTextureId = CTX.textureService.getTextureIndex(data->roughness);
            }
        }
        if (!data->metallic.empty()) {
            auto *tex = CTX.textureService.create(data->metallic);
            if (tex != nullptr) {
                materialData.metallicTextureId = CTX.textureService.getTextureIndex(data->metallic);
            }
        }
        if (!data->height.empty()) {
            auto *tex = CTX.textureService.create(data->height);
            if (tex != nullptr) {
                materialData.heightTextureId = CTX.textureService.getTextureIndex(data->height);
            }
        }

        materials[instance->materialIndex] = materialData;

        auto *materialBuffer = CTX.coreBuffers.materialBuffer;
        materialBuffer->update(materials.data());

        delete data;

        return instance;
    }

    MaterialFileData *MaterialService::stream(const std::string &id) const {
        auto pathToFile = CTX.getAssetDirectory() + FORMAT_FILE_MATERIAL(id);
        if (std::filesystem::exists(pathToFile)) {
            auto *data = new MaterialFileData;
            PARSE_TEMPLATE(*data, pathToFile);
            return data;
        }
        return nullptr;
    }

    unsigned int MaterialService::getMaterialIndex(const std::string &id) {
        if (id.empty()) {
            return 0;
        }
        if (resources.contains(id)) {
            return dynamic_cast<MaterialInstance *>(resources.at(id))->materialIndex;
        }
        auto *instance = create(id);
        if (instance != nullptr) {
            return instance->materialIndex;
        }
        return 0;
    }

    void MaterialService::disposeResource(MaterialInstance *resource) {
        // Materials are currently just entries in a global buffer, 
        // they don't hold many resources themselves, but they do have dependencies
        // which are handled by StreamingService.
    }
} // Metal
