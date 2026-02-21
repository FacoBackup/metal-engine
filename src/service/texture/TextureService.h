#ifndef TEXTURESERVICE_H
#define TEXTURESERVICE_H
#include <string>
#include <sys/types.h>
#include <vulkan/vulkan_core.h>

#include "TextureData.h"
#include "../abstract/AbstractResourceService.h"

namespace Metal {
    struct LevelOfDetail;
    struct TextureInstance;

    class TextureService final : public AbstractResourceService {
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
        [[nodiscard]] TextureData *stream(const std::string &id, const LevelOfDetail &lod) const;

        explicit TextureService()
            : AbstractResourceService() {
        }

        TextureInstance *loadTexture(const std::string &id, const std::string &pathToImage,
                                     bool generateMipMaps = false,
                                     VkFormat imageFormat = VK_FORMAT_R8G8B8A8_SRGB);

        TextureInstance *create(const std::string &id, const LevelOfDetail &lod);

        TextureInstance *createForCompute(unsigned int width, unsigned int height);

        unsigned int getTextureIndex(const std::string &id);
    };
} // Metal

#endif //TEXTURESERVICE_H
