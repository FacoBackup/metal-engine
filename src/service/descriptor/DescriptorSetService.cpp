#include "DescriptorSetService.h"
#include "../../service/pipeline/PipelineBuilder.h"
#include "../../service/buffer/BufferInstance.h"
#include "../../service/log/LogService.h"
#include "../../context/ApplicationContext.h"
#include "../framebuffer/FrameBufferAttachment.h"
#include "../framebuffer/FrameBufferInstance.h"
#include "../texture/TextureInstance.h"
#include "../../util/VulkanUtils.h"

namespace Metal {
    DescriptorInstance *DescriptorSetService::createDescriptor(const PipelineBuilder &pipelineBuilder, const std::string &id, VkShaderStageFlags stageFlags) {
        auto *descriptorInstance = createResourceInstance(id);
        
        for (auto binding : pipelineBuilder.resourceBindings) {
            if (binding.frameBufferId != "") {
                auto *fbo = CTX.framebufferService.getResource(binding.frameBufferId);
                if (fbo == nullptr) {
                    throw std::runtime_error("Framebuffer not found: " + binding.frameBufferId);
                }
                binding.view = fbo->attachments[binding.attachmentIndex]->vkImageView;
                if (binding.sampler == VK_NULL_HANDLE) {
                    binding.sampler = CTX.vulkanContext.vkImageSampler;
                }
            }

            if (binding.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR || binding.accelerationStructure != VK_NULL_HANDLE) {
                binding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
            } else if (binding.bufferInstance != nullptr || !binding.bufferId.empty()) {
                BufferInstance *buffer = binding.bufferInstance;
                if (buffer == nullptr) {
                    buffer = CTX.bufferService.getResource(binding.bufferId);
                }

                if (buffer != nullptr) {
                    binding.descriptorType = buffer->getBufferType() == BufferType::UNIFORM_BUFFER
                                                 ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
                                                 : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                } else {
                    // Default to storage buffer if not found during initialization, or maybe handle differently?
                    // Usually globalData and such are available.
                    binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                }
            } else if (binding.view != VK_NULL_HANDLE) {
                binding.descriptorType = binding.sampler != VK_NULL_HANDLE
                                             ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
                                             : VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            }
            binding.stageFlags = static_cast<VkShaderStageFlagBits>(stageFlags);
            descriptorInstance->bindings.push_back(binding);
        }
        
        updateDescriptor(descriptorInstance);
        return descriptorInstance;
    }

    void DescriptorSetService::disposeResource(DescriptorInstance *resource) {
        LOG_INFO("Disposing of descriptor set resource");
        vkDestroyDescriptorSetLayout(CTX.vulkanContext.device.device, resource->vkDescriptorSetLayout, nullptr);
    }

    std::vector<DescriptorInstance *> DescriptorSetService::getAllDescriptors() const {
        std::vector<DescriptorInstance *> descriptors;
        std::lock_guard lock(resourceMutex);
        for (auto const& [id, descriptor] : resources) {
            descriptors.emplace_back(descriptor);
        }
        return descriptors;
    }

    void DescriptorSetService::setImageDescriptor(const FrameBufferInstance *framebuffer,
                                               unsigned int attachmentIndex) {
        auto attachment = framebuffer->attachments[attachmentIndex];
        if (attachment->imageDescriptor == nullptr) {
            attachment->imageDescriptor = createResourceInstance(std::string(attachment->name));
            attachment->imageDescriptor->bindings.push_back(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0,
                CTX.vulkanContext.vkImageSampler,
                attachment->vkImageView));
            updateDescriptor(attachment->imageDescriptor);
        }
    }

    void DescriptorSetService::setImageDescriptor(TextureInstance *texture) {
        if (texture->imageDescriptor == nullptr) {
            texture->imageDescriptor = createResourceInstance(texture->getId() + "_descriptor");
            texture->imageDescriptor->bindings.push_back(DescriptorBinding::Of(VK_SHADER_STAGE_FRAGMENT_BIT,
                                                                             VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                                             0,
                                                                             texture->vkSampler,
                                                                             texture->vkImageView
            ));
            updateDescriptor(texture->imageDescriptor);
        }
    }
    void DescriptorSetService::updateDescriptor(DescriptorInstance *descriptor) {
        if (descriptor->bindings.empty()) {
            throw std::runtime_error("No descriptor layout sets were created");
        }
        // CREATE
        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
        std::vector<VkDescriptorBindingFlags> bindingFlags;

        for (auto &binding: descriptor->bindings) {
            VkDescriptorSetLayoutBinding &samplerLayoutBinding = descriptorSetLayoutBindings.emplace_back();
            samplerLayoutBinding.binding = binding.bindingPoint;
            samplerLayoutBinding.descriptorType = binding.descriptorType;
            samplerLayoutBinding.descriptorCount = binding.descriptorCount;
            samplerLayoutBinding.stageFlags = binding.stageFlags;
            samplerLayoutBinding.pImmutableSamplers = nullptr;

            if (binding.descriptorCount > 1) {
                bindingFlags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
            } else {
                bindingFlags.push_back(0);
            }
        }

        VkDescriptorSetLayoutBindingFlagsCreateInfo flagsInfo{};
        flagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
        flagsInfo.bindingCount = bindingFlags.size();
        flagsInfo.pBindingFlags = bindingFlags.data();

        VkDescriptorSetLayoutCreateInfo layoutInfo{};

        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = descriptorSetLayoutBindings.size();
        layoutInfo.pBindings = descriptorSetLayoutBindings.data();
        layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;
        layoutInfo.pNext = &flagsInfo;

        VulkanUtils::CheckVKResult(vkCreateDescriptorSetLayout(CTX.vulkanContext.device.device, &layoutInfo,
                                                               nullptr,
                                                               &descriptor->vkDescriptorSetLayout));

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = CTX.vulkanContext.descriptorPool; // Created during setup
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &descriptor->vkDescriptorSetLayout;

        VulkanUtils::CheckVKResult(vkAllocateDescriptorSets(CTX.vulkanContext.device.device, &allocInfo, &descriptor->vkDescriptorSet));

        // WRITE
        Write(descriptor->vkDescriptorSet, descriptor->bindings);
    }

    void DescriptorSetService::write(DescriptorInstance *descriptor) {
        Write(descriptor->vkDescriptorSet, descriptor->bindings);
    }

    void DescriptorSetService::Write(const VkDescriptorSet &vkDescriptorSet,
                                     const std::vector<DescriptorBinding> &bindings) {
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<std::vector<VkDescriptorImageInfo>> imageInfosPool;
        std::vector<VkWriteDescriptorSetAccelerationStructureKHR> asInfos;

        bufferInfos.reserve(bindings.size());
        asInfos.reserve(bindings.size());
        imageInfosPool.reserve(bindings.size());

        for (auto &binding: bindings) {
            if (binding.descriptorType == VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR) {
                if (binding.accelerationStructure != VK_NULL_HANDLE) {
                    VkWriteDescriptorSetAccelerationStructureKHR asInfo{};
                    asInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
                    asInfo.accelerationStructureCount = 1;
                    asInfo.pAccelerationStructures = &binding.accelerationStructure;
                    asInfos.push_back(asInfo);

                    VkWriteDescriptorSetAccelerationStructureKHR *pAsInfo = &asInfos.back();

                    VkWriteDescriptorSet descriptorWrite{};
                    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    descriptorWrite.dstSet = vkDescriptorSet;
                    descriptorWrite.dstBinding = binding.bindingPoint;
                    descriptorWrite.dstArrayElement = 0;
                    descriptorWrite.descriptorType = binding.descriptorType;
                    descriptorWrite.descriptorCount = 1;
                    descriptorWrite.pNext = pAsInfo;

                    writeDescriptorSets.push_back(descriptorWrite);
                }
            } else if (binding.bufferInstance != nullptr || !binding.bufferId.empty()) {
                BufferInstance *buffer = binding.bufferInstance;
                if (buffer == nullptr) {
                    buffer = CTX.bufferService.getResource(binding.bufferId);
                }

                if (buffer == nullptr) {
                    LOG_ERROR("DescriptorSetService Buffer not found: " + binding.bufferId);
                    continue;
                }

                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = buffer->vkBuffer;
                bufferInfo.range = buffer->dataSize;
                bufferInfo.offset = 0;
                // Store the buffer info in the vector.
                bufferInfos.push_back(bufferInfo);

                VkDescriptorBufferInfo *pBufferInfo = &bufferInfos.back();

                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = vkDescriptorSet;
                descriptorWrite.dstBinding = binding.bindingPoint;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType = binding.descriptorType;
                descriptorWrite.descriptorCount = 1;
                // Use the address of the element in the vector.
                descriptorWrite.pBufferInfo = pBufferInfo;

                writeDescriptorSets.push_back(descriptorWrite);
            } else if (binding.view != VK_NULL_HANDLE) {
                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageView = binding.view;
                imageInfo.imageLayout = binding.layout;
                imageInfo.sampler = binding.sampler;

                std::vector<VkDescriptorImageInfo> imageInfos;
                imageInfos.push_back(imageInfo);
                imageInfosPool.push_back(imageInfos);

                VkDescriptorImageInfo *pImageInfo = imageInfosPool.back().data();

                VkWriteDescriptorSet descriptorWrite{};
                descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrite.dstSet = vkDescriptorSet;
                descriptorWrite.dstBinding = binding.bindingPoint;
                descriptorWrite.dstArrayElement = 0;
                descriptorWrite.descriptorType = binding.descriptorType;
                descriptorWrite.descriptorCount = binding.descriptorCount;
                descriptorWrite.pImageInfo = pImageInfo;

                writeDescriptorSets.push_back(descriptorWrite);
            }
        }

        if (writeDescriptorSets.empty()) return;

        vkUpdateDescriptorSets(CTX.vulkanContext.device.device,
                               static_cast<unsigned int>(writeDescriptorSets.size()),
                               writeDescriptorSets.data(),
                               0,
                               nullptr);
    }
}
