#ifndef TEXTUREIMPORTER_H
#define TEXTUREIMPORTER_H
#include "../abstract/AbstractImporter.h"

struct aiTexture;

namespace Metal {
    struct LevelOfDetail;
    struct TextureData;

    class TextureImporterService final : public AbstractImporter {
    public:
        explicit TextureImporterService(ApplicationContext &context)
            : AbstractImporter(context) {
        }

        std::vector<std::string> getSupportedTypes() override {
            return {"png", "jpg", "jpeg"};
        }

        std::string importTexture(const std::string &targetDir, const std::string &pathToFile) const;

        std::string importEmbeddedTexture(const std::string &targetDir, const ::aiTexture *texture,
                                          const std::string &nameHint) const;

        void reduceImage(const std::string &fileId,
                         const TextureData &textureData, const LevelOfDetail &levelOfDetail) const;
    };
}

#endif //TEXTUREIMPORTER_H
