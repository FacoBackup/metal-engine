#ifndef TEXTUREIMPORTER_H
#define TEXTUREIMPORTER_H
#include "AbstractImporter.h"

namespace Metal {
    struct LevelOfDetail;
    struct TextureData;

    class TextureImporter final : public AbstractImporter {
    public:
        explicit TextureImporter(ApplicationContext &context)
            : AbstractImporter(context) {
        }

        std::vector<std::string> getSupportedTypes() override {
            return {"png", "jpg", "jpeg"};
        }

        [[nodiscard]] std::string importTexture(const std::string &targetDir, const std::string &pathToFile) const;

        void reduceImage(const std::string &fileId,
                         const TextureData &textureData, const LevelOfDetail &levelOfDetail) const;
    };
}

#endif //TEXTUREIMPORTER_H
