#include "PipelineBuilder.h"
#include "../texture/TextureInstance.h"

namespace Metal {
    PipelineBuilder PipelineBuilder::Of(std::string frameBufferId, const char *vertexShader,
                                        const char *fragmentShader) {
        PipelineBuilder d{};
        d.frameBufferId = std::move(frameBufferId);
        d.vertexShader = vertexShader;
        d.fragmentShader = fragmentShader;
        return d;
    }

    PipelineBuilder PipelineBuilder::Of(const char *computeShader) {
        PipelineBuilder d{};
        d.computeShader = computeShader;
        return d;
    }

    PipelineBuilder PipelineBuilder::OfRayTracing(const char *rayGen, const char *miss, const char *closestHit) {
        PipelineBuilder d{};
        d.rayGenShader = rayGen;
        d.missShader = miss;
        d.closestHitShader = closestHit;
        d.isRayTracing = true;
        return d;
    }

    PipelineBuilder &PipelineBuilder::setBlendEnabled() {
        blendEnabled = true;
        return *this;
    }

    PipelineBuilder &PipelineBuilder::setPrepareForMesh() {
        prepareForMesh = true;
        return *this;
    }

    PipelineBuilder &PipelineBuilder::setDepthTest() {
        depthTest = true;
        return *this;
    }

    PipelineBuilder &PipelineBuilder::useTriangleStrip() {
        useStrip = true;
        return *this;
    }

    PipelineBuilder &PipelineBuilder::setCullMode(VkCullModeFlagBits cullMode) {
        this->cullMode = cullMode;
        return *this;
    }

    PipelineBuilder &PipelineBuilder::setPushConstantsSize(unsigned int size) {
        pushConstantsSize = size;
        return *this;
    }

    PipelineBuilder &PipelineBuilder::addResourceBinding(std::string bufferId) {
        DescriptorBinding b{};
        b.bindingPoint = currentBindingPoint++;
        b.bufferId = std::move(bufferId);
        resourceBindings.push_back(b);
        return *this;
    }

    PipelineBuilder &PipelineBuilder::addResourceBinding(VkSampler sampler, VkImageView view,
                                                         VkImageLayout layout,
                                                         unsigned int descriptorCount) {
        DescriptorBinding b{};
        b.bindingPoint = currentBindingPoint++;
        b.sampler = sampler;
        b.view = view;
        b.layout = layout;
        b.descriptorCount = descriptorCount;
        resourceBindings.push_back(b);
        return *this;
    }

    PipelineBuilder &PipelineBuilder::addResourceBinding(std::string frameBufferId, uint32_t attachmentIndex,
                                                         VkImageLayout layout) {
        DescriptorBinding b{};
        b.bindingPoint = currentBindingPoint++;
        b.frameBufferId = std::move(frameBufferId);
        b.attachmentIndex = static_cast<int>(attachmentIndex);
        b.layout = layout;
        resourceBindings.push_back(b);
        return *this;
    }

    PipelineBuilder &PipelineBuilder::addResourceBinding(TextureInstance *texture) {
        return addResourceBinding(texture->vkSampler, texture->vkImageView);
    }

    PipelineBuilder &PipelineBuilder::addResourceBinding(const VkImageView imageView) {
        DescriptorBinding b{};
        b.bindingPoint = currentBindingPoint++;
        b.view = imageView;
        b.layout = VK_IMAGE_LAYOUT_GENERAL;
        resourceBindings.push_back(b);
        return *this;
    }

    PipelineBuilder &PipelineBuilder::addResourceBinding(const VkAccelerationStructureKHR tlas) {
        DescriptorBinding b{};
        b.bindingPoint = currentBindingPoint++;
        b.accelerationStructure = tlas;
        b.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
        resourceBindings.push_back(b);
        return *this;
    }

    PipelineBuilder &PipelineBuilder::setId(const char *id) {
        this->id = id;
        return *this;
    }
}
