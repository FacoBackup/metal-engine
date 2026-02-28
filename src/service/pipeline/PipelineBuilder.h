#ifndef PIPELINEBUILDER_H
#define PIPELINEBUILDER_H
#include <vector>
#include <vulkan/vulkan.h>
#include "../descriptor/DescriptorBinding.h"

namespace Metal {
    struct DescriptorInstance;
    struct FrameBufferInstance;
    struct TextureInstance;

    struct PipelineBuilder final {
        const char *id = nullptr;
        FrameBufferInstance *frameBuffer = nullptr;
        VkCullModeFlagBits cullMode = VK_CULL_MODE_NONE;
        const char *vertexShader = nullptr;
        const char *fragmentShader = nullptr;
        const char *computeShader = nullptr;
        const char *rayGenShader = nullptr;
        const char *missShader = nullptr;
        const char *closestHitShader = nullptr;
        std::vector<DescriptorBinding> resourceBindings{};
        unsigned int pushConstantsSize = 0;
        unsigned int currentBindingPoint = 0;
        bool blendEnabled = false;
        bool prepareForMesh = false;
        bool depthTest = false;
        bool useStrip = false;
        bool isRayTracing = false;

        static PipelineBuilder Of(FrameBufferInstance *frameBuffer, const char *vertexShader,
                                  const char *fragmentShader);

        static PipelineBuilder Of(const char *computeShader);

        static PipelineBuilder OfRayTracing(const char *rayGen, const char *miss, const char *closestHit);

        PipelineBuilder &setBlendEnabled();

        PipelineBuilder &setPrepareForMesh();

        PipelineBuilder &setDepthTest();

        PipelineBuilder &useTriangleStrip();

        PipelineBuilder &setCullMode(VkCullModeFlagBits cullMode);

        PipelineBuilder &setPushConstantsSize(unsigned int size);

        PipelineBuilder &addResourceBinding(BufferInstance *buffer);

        PipelineBuilder &addResourceBinding(VkSampler sampler, VkImageView view,
                                            VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                            unsigned int descriptorCount = 1);

        PipelineBuilder &addResourceBinding(TextureInstance *texture);

        PipelineBuilder &addResourceBinding(VkImageView imageView);

        PipelineBuilder &addResourceBinding(VkAccelerationStructureKHR tlas);

        PipelineBuilder &setId(const char *id);
    };
}

#endif //PIPELINEBUILDER_H
