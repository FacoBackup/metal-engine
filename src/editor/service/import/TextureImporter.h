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

        static void importTexture(const std::string &targetDir, const std::string &pathToFile);

        static void reduceImage(std::vector<std::string> &paths, const std::string &fileId,
                                const std::string &targetDir,
                                const TextureData &textureData, const LevelOfDetail &levelOfDetail);
    };
}

#endif //TEXTUREIMPORTER_H
