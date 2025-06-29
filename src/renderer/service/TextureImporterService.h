#ifndef TEXTUREIMPORTER_H
#define TEXTUREIMPORTER_H
#include "../../common/AbstractImporter.h"

namespace Metal {
    struct LevelOfDetail;
    struct TextureData;

    class TextureImporterService final : public AbstractImporter {
    public:
        std::vector<std::string> getSupportedTypes() override {
            return {"png", "jpg", "jpeg"};
        }

        std::string importTexture(const std::string &targetDir, const std::string &pathToFile) const;

        void reduceImage(const std::string &fileId,
                         const TextureData &textureData, const LevelOfDetail &levelOfDetail) const;
    };
}

#endif //TEXTUREIMPORTER_H
