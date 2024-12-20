#include "TextureImporter.h"

#include "stb_image.h"
#include "stb_image_write.h"
#include <filesystem>

#include "../../../common/util/files/FileMetadata.h"
#include "../../../common/util/files/EntryType.h"
#include "../../../common/util/files/FilesUtil.h"
#include "../../../context/ApplicationContext.h"
#include "../../../context/runtime/assets/TextureData.h"
#include "../../../engine/enum/LevelOfDetail.h"

namespace fs = std::filesystem;

namespace Metal {
    std::string TextureImporter::importTexture(const std::string &targetDir, const std::string &pathToFile) const {
        auto metadata = FileMetadata{};
        metadata.type = EntryType::TEXTURE;
        metadata.name = fs::path(pathToFile).filename().string();
        metadata.name = metadata.name.substr(0, metadata.name.find_last_of('.'));

        int width, height, channels;
        unsigned char *data = stbi_load(pathToFile.c_str(), &width, &height, &channels, 0);
        if (!data) {
            throw std::runtime_error("Failed to load image: " + pathToFile);
        }
        const auto textureData = TextureData{width, height, channels, data};
        FilesUtil::WriteFile((targetDir + '/' + FORMAT_FILE_METADATA(metadata.getId())).c_str(),
                             metadata.serialize().c_str());

        reduceImage(metadata.getId(), textureData, LevelOfDetail::LOD_0);
        reduceImage(metadata.getId(), textureData, LevelOfDetail::LOD_1);
        reduceImage(metadata.getId(), textureData, LevelOfDetail::LOD_2);
        reduceImage(metadata.getId(), textureData, LevelOfDetail::LOD_3);

        stbi_image_free(textureData.data);
        return metadata.getId();
    }

    void TextureImporter::reduceImage(const std::string &fileId,
                                      const TextureData &textureData, const LevelOfDetail &levelOfDetail) const {
        const int newWidth = textureData.width / levelOfDetail.level;
        const int newHeight = textureData.height / levelOfDetail.level;

        // Allocate memory for the resized image
        auto *resizedData = new unsigned char[newWidth * newHeight * textureData.channels];

        // Resize the image
        for (int y = 0; y < newHeight; ++y) {
            for (int x = 0; x < newWidth; ++x) {
                int srcX = x * textureData.width / newWidth;
                int srcY = y * textureData.height / newHeight;
                for (int c = 0; c < textureData.channels; ++c) {
                    resizedData[(y * newWidth + x) * textureData.channels + c] = textureData.data[
                        (srcY * textureData.width + srcX) * textureData.channels + c];
                }
            }
        }

        if (!stbi_write_png((context.getAssetDirectory() + FORMAT_FILE_TEXTURE(fileId, levelOfDetail)).c_str(),
                            newWidth, newHeight, textureData.channels, resizedData,
                            newWidth * textureData.channels)) {
            stbi_image_free(textureData.data);
            delete[] resizedData;
            throw std::runtime_error("Failed to write resized image");
        }

        delete[] resizedData;
    }
} // Metal
