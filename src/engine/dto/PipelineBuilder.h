#ifndef PIPELINEBUILDER_H
#define PIPELINEBUILDER_H
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include "DescriptorBinding.h"

namespace Metal {
    struct DescriptorInstance;
    struct FrameBufferInstance;
    struct TextureInstance;

    /**
     * @struct DescriptorBindingBuilder
     * @brief Helper structure used by PipelineBuilder to define a single descriptor binding.
     *
     * It stores resource identifiers or Vulkan handles, binding points, and types
     * required to create descriptor sets for a pipeline.
     */
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

    /**
     * @struct PipelineBuilder
     * @brief Fluent builder for configuring and creating Vulkan pipelines.
     *
     * Supports Graphics, Compute, and Ray Tracing pipelines. It collects shader stages,
     * render state (blending, depth test, culling), and resource bindings.
     */
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

        /**
         * @brief Creates a builder for a graphics pipeline.
         */
        static PipelineBuilder Of(std::string frameBufferId, const char *vertexShader,
                                  const char *fragmentShader);

        /**
         * @brief Creates a builder for a compute pipeline.
         */
        static PipelineBuilder Of(const char *computeShader);

        /**
         * @brief Creates a builder for a ray tracing pipeline.
         */
        static PipelineBuilder OfRayTracing(const char *rayGen, const char *miss, const char *closestHit);

        /**
         * @brief Enables alpha blending for the pipeline.
         */
        PipelineBuilder &enableBlending();

        /**
         * @brief Prepares the pipeline for primitive rendering (sets vertex input/mesh flags).
         */
        PipelineBuilder &enablePrimitiveRendering();

        /**
         * @brief Enables depth testing and writing.
         */
        PipelineBuilder &enableDepthTest();

        /**
         * @brief Configures the pipeline to use triangle strips.
         */
        PipelineBuilder &useTriangleStrip();

        /**
         * @brief Sets the rasterization culling mode.
         */
        PipelineBuilder &setCullMode(VkCullModeFlagBits cullMode);

        /**
         * @brief Sets the size of the push constant range.
         */
        PipelineBuilder &setPushConstantsSize(unsigned int size);

        /**
         * @brief Adds a uniform/storage buffer binding.
         */
        PipelineBuilder &addBufferBinding(std::string bufferId);

        /**
         * @brief Adds a combined image sampler binding with specific handles.
         */
        PipelineBuilder &addCombinedImageSamplerBinding(VkSampler sampler, VkImageView view,
                                            VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        /**
         * @brief Adds a single combined image sampler binding.
         */
        PipelineBuilder &addSingleCombinedImageSamplerBinding(VkSampler sampler, VkImageView view,
                                                        VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        /**
         * @brief Adds a binding for a specific framebuffer attachment. Attachments are not persisted as textures
         */
        PipelineBuilder &addFboBinding(std::string frameBufferId, uint32_t attachmentIndex,
                                            VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        /**
         * @brief Adds a binding for an existing texture instance.
         */
        PipelineBuilder &addTextureBinding(TextureInstance *texture);

        /**
         * @brief Adds a storage image binding for compute or ray tracing.
         */
        PipelineBuilder &addStorageImageBinding(std::string storageImageId);

        /**
         * @brief Adds an acceleration structure binding for ray tracing.
         */
        PipelineBuilder &addAccelerationStructureBinding(VkAccelerationStructureKHR tlas);

        /**
         * @brief Sets the unique ID for the pipeline.
         */
        PipelineBuilder &setId(const char *id);
    };
}

#endif //PIPELINEBUILDER_H
