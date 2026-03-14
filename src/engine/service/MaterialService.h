#ifndef MATERIALSERVICE_H
#define MATERIALSERVICE_H
#include "../../common/IService.h"

namespace Metal {
    struct PrimitiveComponent;
    struct MeshMetadata;
    class TextureService;

    class MaterialService final : public IService {
        TextureService *textureService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"TextureService", &textureService}
            };
        }

        void load(MeshMetadata &data, PrimitiveComponent &component);
    };
} // Metal

#endif //MATERIALSERVICE_H
