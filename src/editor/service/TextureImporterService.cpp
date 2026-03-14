#include "TextureImporterService.h"
#include "../../core/DirectoryService.h"
#include "../../common/LoggerUtil.h"
#include "../enum/engine-definitions.h"

#include "../../../../dependencies/stb/stb_image.h"
#include "../../../../dependencies/stb/stb_image_write.h"
#include <assimp/texture.h>
#include <filesystem>
#include "../../common/serialization-definitions.h"

#include "../dto/EntryMetadata.h"
#include "../enum/EntryType.h"
#include "../../common/FilesUtil.h"
#include "../../engine/dto/TextureData.h"

namespace fs = std::filesystem;

namespace Metal {
    std::string TextureImporterService::importData(const std::string &targetDir,
                                            const std::string &pathToFile, const std::shared_ptr<ImportSettingsDTO> &settings, const std::stop_token &stopToken) {
        try {
            auto metadata = EntryMetadata{};
            metadata.type = EntryType::TEXTURE;
            metadata.name = fs::path(pathToFile).filename().string();
            metadata.name = metadata.name.substr(0, metadata.name.find_last_of('.'));

            int width, height, channels;
            unsigned char *data = stbi_load(pathToFile.c_str(), &width, &height, &channels, 0);
            if (!data) {
                throw std::runtime_error("Failed to load image: " + pathToFile);
            }
            const auto textureData = TextureData{width, height, channels, data};

            metadata.size += saveImage(metadata.getId(), textureData);

            DUMP_TEMPLATE(targetDir + '/' + FORMAT_FILE_METADATA(metadata.getId()), metadata)

            stbi_image_free(textureData.data);
            return metadata.getId();
        } catch (std::exception &e) {
            LOG_ERROR(std::string("Texture import failed: ") + e.what());
            throw std::runtime_error("Texture import failed");
        }
    }

    std::string TextureImporterService::importEmbeddedTexture(const std::string &targetDir, const ::aiTexture *texture,
                                                              const std::string &nameHint) const {
        if (!texture) return "";
        try {
            auto metadata = EntryMetadata{};
            metadata.type = EntryType::TEXTURE;
            metadata.name = nameHint.empty() ? "embedded" : nameHint;
            DUMP_TEMPLATE(targetDir + '/' + FORMAT_FILE_METADATA(metadata.getId()), metadata)

            int width = 0;
            int height = 0;
            int channels = 0;

            unsigned char *data = nullptr;
            std::vector<unsigned char> owned;

            if (texture->mHeight == 0) {
                // Compressed texture data (common in glTF/GLB). mWidth holds the byte length.
                data = stbi_load_from_memory(reinterpret_cast<const stbi_uc *>(texture->pcData),
                                             static_cast<int>(texture->mWidth),
                                             &width, &height, &channels, 0);
                if (!data) {
                    throw std::runtime_error("Failed to decode embedded (compressed) texture");
                }
            } else {
                // Raw texels.
                width = static_cast<int>(texture->mWidth);
                height = static_cast<int>(texture->mHeight);
                channels = 4;
                owned.resize(static_cast<size_t>(width) * static_cast<size_t>(height) * 4u);
                for (int y = 0; y < height; ++y) {
                    for (int x = 0; x < width; ++x) {
                        const auto &t = texture->pcData[static_cast<unsigned int>(y * width + x)];
                        const size_t idx = (static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(
                                                x)) * 4u;
                        owned[idx + 0] = t.r;
                        owned[idx + 1] = t.g;
                        owned[idx + 2] = t.b;
                        owned[idx + 3] = t.a;
                    }
                }
                data = owned.data();
            }

            const auto textureData = TextureData{width, height, channels, data};
            metadata.size += saveImage(metadata.getId(), textureData);

            DUMP_TEMPLATE(targetDir + '/' + FORMAT_FILE_METADATA(metadata.getId()), metadata)

            if (texture->mHeight == 0) {
                stbi_image_free(data);
            }
            return metadata.getId();
        } catch (std::exception &e) {
            LOG_ERROR(std::string("Embedded texture import failed: ") + e.what());
            return "";
        }
    }

    size_t TextureImporterService::saveImage(const std::string &fileId,
                                             const TextureData &textureData) const {
        std::string texturePath = directoryService->getAssetDirectory() + FORMAT_FILE_TEXTURE(fileId);
        if (!stbi_write_png(texturePath.c_str(),
                            textureData.width, textureData.height, textureData.channels, textureData.data,
                            textureData.width * textureData.channels)) {
            throw std::runtime_error("Failed to write image");
        }

        return fs::file_size(texturePath);
    }
} // Metal
