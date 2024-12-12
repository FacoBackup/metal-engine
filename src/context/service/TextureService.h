#ifndef TEXTURESERVICE_H
#define TEXTURESERVICE_H
#include <string>
#include <vulkan/vulkan_core.h>

#include "../../common/interface/AbstractResourceService.h"

namespace Metal {
    struct TextureInstance;

    class TextureService final : public AbstractResourceService {
        VkSampler createVkSampler() const;

        VkImageView createVkImageView(VkImage image, VkFormat format) const;

    public:
        explicit TextureService(ApplicationContext &context)
            : AbstractResourceService(context) {
        }

        [[nodiscard]] TextureInstance *loadTexture(const std::string &pathToImage) const;

        void createVkImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                         VkImageUsageFlags usage,
                         VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory) const;
    };
} // Metal

#endif //TEXTURESERVICE_H
