#ifndef MATERIALSERVICE_H
#define MATERIALSERVICE_H
#include "../../common/IService.h"

namespace Metal {
    struct WorldRepository;
    struct PrimitiveComponent;
    struct MeshMetadata;
    class TextureService;

    class MaterialService final : public IService {
        TextureService *textureService = nullptr;
        WorldRepository *worldRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"TextureService", &textureService},
                {"WorldRepository", &worldRepository}
            };
        }

        void load(MeshMetadata &data, PrimitiveComponent &component);
    };
} // Metal

#endif //MATERIALSERVICE_H
