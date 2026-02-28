#ifndef PIPELINEBUILDER_H
#define PIPELINEBUILDER_H
#include <vector>
#include <vulkan/vulkan.h>

namespace Metal {
    struct DescriptorInstance;
    struct FrameBufferInstance;

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
        std::vector<DescriptorInstance *> descriptorSetsToBind{};
        unsigned int pushConstantsSize = 0;
        bool blendEnabled = false;
        bool prepareForMesh = false;
        bool depthTest = false;
        bool useStrip = false;
        bool isRayTracing = false;

        static PipelineBuilder Of(FrameBufferInstance *frameBuffer, const char *vertexShader,
                                  const char *fragmentShader) {
            PipelineBuilder d{};
            d.frameBuffer = frameBuffer;
            d.vertexShader = vertexShader;
            d.fragmentShader = fragmentShader;
            return d;
        }

        static PipelineBuilder Of(const char *computeShader) {
            PipelineBuilder d{};
            d.computeShader = computeShader;
            return d;
        }

        static PipelineBuilder OfRayTracing(const char *rayGen, const char *miss, const char *closestHit) {
            PipelineBuilder d{};
            d.rayGenShader = rayGen;
            d.missShader = miss;
            d.closestHitShader = closestHit;
            d.isRayTracing = true;
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

        PipelineBuilder &useTriangleStrip() {
            useStrip = true;
            return *this;
        }

        PipelineBuilder &setCullMode(VkCullModeFlagBits cullMode) {
            this->cullMode = cullMode;
            return *this;
        }

        PipelineBuilder &setPushConstantsSize(unsigned int size) {
            pushConstantsSize = size;
            return *this;
        }

        PipelineBuilder &addDescriptorSet(DescriptorInstance *d) {
            descriptorSetsToBind.push_back(d);
            return *this;
        }

        PipelineBuilder &setId(const char *id) {
            this->id = id;
            return *this;
        }
    };
}

#endif //PIPELINEBUILDER_H
