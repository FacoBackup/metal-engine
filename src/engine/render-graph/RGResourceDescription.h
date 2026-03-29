#ifndef RGRESOURCEDESCRIPTION_H
#define RGRESOURCEDESCRIPTION_H

#include <string>
#include <vulkan/vulkan.h>
#include <glm/vec4.hpp>
#include "../enum/ResourceType.h"
#include "../resource/BufferInstance.h"

namespace Metal {
    /**
     * @struct RGResourceDescription
     * @brief Describes properties of a Render Graph resource.
     */
    struct RGResourceDescription {
        std::string id;
        ResourceType type;

        // Image description
        unsigned int width = 0;
        unsigned int height = 0;
        VkFormat format = VK_FORMAT_UNDEFINED;
        VkImageUsageFlags usage = 0;
        glm::vec4 clearColor = glm::vec4(0.0f);
        bool hasDepth = false;

        // Buffer description
        VkDeviceSize bufferSize = 0;
        VkBufferUsageFlags bufferUsage = 0;
        VkMemoryPropertyFlags bufferProperties = 0;
        BufferType bufferType = UNIFORM_BUFFER;

        // For RenderTargets
        struct Attachment {
            VkFormat format;
            VkImageUsageFlags usage;
        };
        std::vector<Attachment> colorAttachments;
    };
}

#endif // RGRESOURCEDESCRIPTION_H
