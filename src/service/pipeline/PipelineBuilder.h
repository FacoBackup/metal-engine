#ifndef PIPELINEBUILDER_H
#define PIPELINEBUILDER_H
#include <vector>
#include <vulkan/vulkan.h>

namespace Metal {
    struct DescriptorInstance;
    struct FrameBufferInstance;

    struct PipelineBuilder final {
        FrameBufferInstance *frameBuffer = nullptr;
        VkCullModeFlagBits cullMode = VK_CULL_MODE_NONE;
        const char *vertexShader = nullptr;
        const char *fragmentShader = nullptr;
        std::vector<DescriptorInstance *> descriptorSetsToBind{};
        uint32_t pushConstantsSize = 0;
        bool blendEnabled = false;
        bool prepareForMesh = false;
        bool depthTest = false;

        static PipelineBuilder Of(FrameBufferInstance *frameBuffer, const char *vertexShader,
                                  const char *fragmentShader) {
            PipelineBuilder d{};
            d.frameBuffer = frameBuffer;
            d.vertexShader = vertexShader;
            d.fragmentShader = fragmentShader;
            return d;
        }

        PipelineBuilder &setBlendEnabled() {
            blendEnabled = true;
            return *this;
        }

        PipelineBuilder &setPrepareForMesh() {
            prepareForMesh = true;
            return *this;
        }

        PipelineBuilder &setDepthTest() {
            depthTest = true;
            return *this;
        }

        PipelineBuilder &setCullMode(VkCullModeFlagBits cullMode) {
            this->cullMode = cullMode;
            return *this;
        }

        PipelineBuilder &setPushConstantsSize(uint32_t size) {
            pushConstantsSize = size;
            return *this;
        }

        PipelineBuilder &addDescriptorSet(DescriptorInstance *d) {
            descriptorSetsToBind.push_back(d);
            return *this;
        }
    };
}

#endif //PIPELINEBUILDER_H
