#ifndef TEXTUREIMPORTER_H
#define TEXTUREIMPORTER_H
#include "../../common/AbstractImporter.h"

struct aiTexture;

namespace Metal {
    class DirectoryService;
    struct TextureData;

    class TextureImporterService final : public AbstractImporter {
        DirectoryService *directoryService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"DirectoryService", directoryService}
            };
        }

        std::vector<std::string> getSupportedTypes() override {
            return {"png", "jpg", "jpeg"};
        }

        std::string importData(const std::string &targetDir, const std::string &pathToFile, const std::shared_ptr<ImportSettingsDTO> &settings, const std::stop_token &stopToken) override;

        std::string importEmbeddedTexture(const std::string &targetDir, const ::aiTexture *texture,
                                          const std::string &nameHint) const;

        size_t saveImage(const std::string &fileId, const TextureData &textureData) const;
    };
}

#endif //TEXTUREIMPORTER_H
