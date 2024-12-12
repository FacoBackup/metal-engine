#include "TextureImporter.h"

#include "stb_image.h"
#include "stb_image_write.h"
#include <filesystem>

#include "FileMetadata.h"
#include "TextureData.h"
#include "../../../common/util/files/EntryType.h"
#include "../../../context/ApplicationContext.h"
#include "../../../engine/LevelOfDetail.h"

namespace fs = std::filesystem;

namespace Metal {
    void TextureImporter::importTexture(const std::string &targetDir, const std::string &pathToFile) {
        auto metadata = FileMetadata{};

        int width, height, channels;
        unsigned char *data = stbi_load(pathToFile.c_str(), &width, &height, &channels, 0);
        if (!data) {
            throw std::runtime_error("Failed to load image: " + pathToFile);
        }
        const auto textureData = TextureData{width, height, channels, data};

        reduceImage(metadata.associatedFiles, metadata.getId(), targetDir, textureData, LevelOfDetail::LOD_0);
        reduceImage(metadata.associatedFiles, metadata.getId(), targetDir, textureData, LevelOfDetail::LOD_1);
        reduceImage(metadata.associatedFiles, metadata.getId(), targetDir, textureData, LevelOfDetail::LOD_2);
        reduceImage(metadata.associatedFiles, metadata.getId(), targetDir, textureData, LevelOfDetail::LOD_3);
        metadata.serialize(targetDir + '/' + metadata.getId() + FILE_METADATA);
    }

    void TextureImporter::reduceImage(std::vector<std::string> &paths, const std::string &fileId,
                                      const std::string &targetDir,
                                      const TextureData &textureData, const LevelOfDetail &levelOfDetail) {
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
                    resizedData[(y * newWidth + x) * textureData.channels + c] = textureData.data[(srcY * textureData.width + srcX) * textureData.channels + c];
                }
            }
        }

        const std::string newPath = targetDir + "/" +
                                    LevelOfDetail::GetFormattedName(fileId, levelOfDetail, EntryType::TEXTURE);
        paths.push_back(newPath);
        if (!stbi_write_png(newPath.c_str(), newWidth, newHeight, textureData.channels, resizedData, newWidth *textureData. channels)) {
            stbi_image_free(textureData.data);
            delete[] resizedData;
            throw std::runtime_error("Failed to write resized image: " + newPath);
        }

        stbi_image_free(textureData.data);
        delete[] resizedData;
    }
} // Metal
