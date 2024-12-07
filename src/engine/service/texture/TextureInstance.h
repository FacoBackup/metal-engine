#ifndef TEXTUREINSTANCE_H
#define TEXTUREINSTANCE_H
#include <vulkan/vulkan_core.h>

#include "../../repository/resource/RuntimeResource.h"

namespace Metal {
    struct TextureInstance : RuntimeResource{
        VkImage image = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
    };
} // Metal

#endif //TEXTUREINSTANCE_H
