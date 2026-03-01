#ifndef PIPELINEBUILDER_H
#define PIPELINEBUILDER_H
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include "../descriptor/DescriptorBinding.h"

namespace Metal {
    struct DescriptorInstance;
    struct FrameBufferInstance;
    struct TextureInstance;

    struct DescriptorBindingBuilder {
        std::string bufferId;
        std::string storageImageId;
        VkSampler sampler = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VkImageLayout layout = VK_IMAGE_LAYOUT_GENERAL;
        VkAccelerationStructureKHR accelerationStructure = VK_NULL_HANDLE;
        std::string frameBufferId;
        int attachmentIndex = -1;
        unsigned int descriptorCount = 1;
        unsigned int bindingPoint = 0;
        VkDescriptorType descriptorType = static_cast<VkDescriptorType>(0);
        DescriptorBindingType type;
    };

    struct PipelineBuilder final {
        const char *id = nullptr;
        std::string frameBufferId;
        VkCullModeFlagBits cullMode = VK_CULL_MODE_NONE;
        const char *vertexShader = nullptr;
        const char *fragmentShader = nullptr;
        const char *computeShader = nullptr;
        const char *rayGenShader = nullptr;
        const char *missShader = nullptr;
        const char *closestHitShader = nullptr;
        std::vector<DescriptorBindingBuilder> resourceBindings{};
        unsigned int pushConstantsSize = 0;
        unsigned int currentBindingPoint = 0;
        bool blendEnabled = false;
        bool prepareForMesh = false;
        bool depthTest = false;
        bool useStrip = false;
        bool isRayTracing = false;

        static PipelineBuilder Of(std::string frameBufferId, const char *vertexShader,
                                  const char *fragmentShader);

        static PipelineBuilder Of(const char *computeShader);

        static PipelineBuilder OfRayTracing(const char *rayGen, const char *miss, const char *closestHit);

        PipelineBuilder &setBlendEnabled();

        PipelineBuilder &setPrepareForMesh();

        PipelineBuilder &setDepthTest();

        PipelineBuilder &useTriangleStrip();

        PipelineBuilder &setCullMode(VkCullModeFlagBits cullMode);

        PipelineBuilder &setPushConstantsSize(unsigned int size);

        PipelineBuilder &addBufferBinding(std::string bufferId);

        PipelineBuilder &addCombinedImageSamplerBinding(VkSampler sampler, VkImageView view,
                                            VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                            unsigned int descriptorCount = 1);

        PipelineBuilder &addFboBinding(std::string frameBufferId, uint32_t attachmentIndex,
                                            VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        PipelineBuilder &addTextureBinding(TextureInstance *texture);

        PipelineBuilder &addStorageImageBinding(std::string storageImageId);

        PipelineBuilder &addAccelerationStructureBinding(VkAccelerationStructureKHR tlas);

        PipelineBuilder &setId(const char *id);
    };
}

#endif //PIPELINEBUILDER_H
