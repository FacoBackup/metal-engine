#ifndef TEXTURESERVICE_H
#define TEXTURESERVICE_H
#include <string>
#include <sys/types.h>
#include <vulkan/vulkan_core.h>

#include "TextureData.h"
#include "../abstract/AbstractResourceService.h"
#include "TextureInstance.h"

namespace Metal {
    struct TextureInstance;

    class TextureService final : public AbstractResourceService<TextureInstance> {
        unsigned int nextTextureIndex = 1;
        std::unordered_map<std::string, unsigned int> textureIndices{};

        void copyBufferToImage(const VkBuffer &vkBuffer, const TextureInstance *image, int layerCount) const;

        void createImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlagBits vkMemoryProperties,
                                 TextureInstance *image) const;

        void createSampler(TextureInstance *image) const;

        void createImageView(VkFormat imageFormat, TextureInstance *image) const;

        void createImage(VkFormat imageFormat, TextureInstance *image, int width, int height) const;

        void transitionImageLayout(const TextureInstance *image, VkImageLayout oldLayout,
                                   VkImageLayout newLayout) const;

        void generateMipmaps(const TextureInstance *image) const;

    public:
        [[nodiscard]] TextureData *loadTextureData(const std::string &id) const;

        TextureInstance *stream(const std::string &id);

        TextureInstance *loadTexture(const std::string &id, const std::string &pathToImage,
                                     bool generateMipMaps = false,
                                     VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB);

        TextureInstance *create(const std::string &id);

        TextureInstance *createForCompute(const std::string &id, unsigned int width, unsigned int height, VkFormat format = VK_FORMAT_R16G16B16A16_SFLOAT);

        unsigned int getTextureIndex(const std::string &id);

        void disposeResource(TextureInstance *resource) override;

        void createSampler(bool linear, VkSampler &vkImageSampler, VkSamplerAddressMode addressMode);
    };
} // Metal

#endif //TEXTURESERVICE_H
