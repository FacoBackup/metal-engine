#include "PipelineBuilder.h"
#include "../resource/TextureInstance.h"

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

    PipelineBuilder &PipelineBuilder::enableBlending() {
        blendEnabled = true;
        return *this;
    }

    PipelineBuilder &PipelineBuilder::enablePrimitiveRendering() {
        prepareForMesh = true;
        return *this;
    }

    PipelineBuilder &PipelineBuilder::enableDepthTest() {
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

    PipelineBuilder &PipelineBuilder::addBufferBinding(std::string bufferId) {
        DescriptorBindingBuilder b{};
        b.bindingPoint = currentBindingPoint++;
        b.bufferId = std::move(bufferId);
        b.type = DescriptorBindingType::BUFFER;
        resourceBindings.push_back(b);
        return *this;
    }

    PipelineBuilder &PipelineBuilder::addCombinedImageSamplerBinding(VkSampler sampler, VkImageView view,
                                                         VkImageLayout layout) {
        DescriptorBindingBuilder b{};
        b.bindingPoint = currentBindingPoint++;
        b.sampler = sampler;
        b.view = view;
        b.layout = layout;
        b.descriptorCount = 0; // Use 0 to indicate dynamic query from VulkanContext
        b.type = DescriptorBindingType::COMBINED_IMAGE_SAMPLER;
        resourceBindings.push_back(b);
        return *this;
    }

    PipelineBuilder &PipelineBuilder::addFboBinding(std::string frameBufferId, uint32_t attachmentIndex,
                                                         VkImageLayout layout) {
        DescriptorBindingBuilder b{};
        b.bindingPoint = currentBindingPoint++;
        b.frameBufferId = std::move(frameBufferId);
        b.attachmentIndex = static_cast<int>(attachmentIndex);
        b.layout = layout;
        b.type = DescriptorBindingType::FBO_ATTACHMENT;
        resourceBindings.push_back(b);
        return *this;
    }

    PipelineBuilder &PipelineBuilder::addTextureBinding(TextureInstance *texture) {
        return addCombinedImageSamplerBinding(texture->vkSampler, texture->vkImageView);
    }

    PipelineBuilder &PipelineBuilder::addStorageImageBinding(std::string storageImageId) {
        DescriptorBindingBuilder b{};
        b.bindingPoint = currentBindingPoint++;
        b.storageImageId = std::move(storageImageId);
        b.layout = VK_IMAGE_LAYOUT_GENERAL;
        b.type = DescriptorBindingType::STORAGE_IMAGE;
        resourceBindings.push_back(b);
        return *this;
    }

    PipelineBuilder &PipelineBuilder::addAccelerationStructureBinding(const VkAccelerationStructureKHR tlas) {
        DescriptorBindingBuilder b{};
        b.bindingPoint = currentBindingPoint++;
        b.accelerationStructure = tlas;
        b.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
        b.type = DescriptorBindingType::ACCELERATION_STRUCTURE;
        resourceBindings.push_back(b);
        return *this;
    }

    PipelineBuilder &PipelineBuilder::setId(const char *id) {
        this->id = id;
        return *this;
    }
}
