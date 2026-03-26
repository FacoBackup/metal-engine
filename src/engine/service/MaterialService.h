#ifndef MATERIALSERVICE_H
#define MATERIALSERVICE_H
#include "../../common/IService.h"
#include "../../common/IInit.h"
#include "../../common/ISync.h"
#include "../../common/IEventMember.h"
#include "../dto/MaterialData.h"

namespace Metal {
    struct MaterialData;
    struct PrimitiveComponent;
    class TextureService;
    class EngineContext;
    struct WorldRepository;
    class BufferService;
    class DirtyStateService;

    class MaterialService final : public IService, public IInit, public ISync, public IEventMember {
        TextureService *textureService = nullptr;
        EngineContext *engineContext = nullptr;
        WorldRepository *worldRepository = nullptr;
        BufferService *bufferService = nullptr;
        DirtyStateService *dirtyStateService = nullptr;

        std::vector<MaterialData> materialData{};
        bool fullRebuildNeeded = false;

        /**
         * @brief Performs a full rebuild of the material data.
         * @return True if any material data was changed.
         */
        bool rebuildMaterialData();

        /**
         * @brief Processes entities marked as dirty for material updates.
         * @return True if any material data was changed.
         */
        bool processDirtyMaterials();

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"TextureService", &textureService},
                {"EngineContext", &engineContext},
                {"WorldRepository", &worldRepository},
                {"BufferService", &bufferService},
                {"DirtyStateService", &dirtyStateService}
            };
        }

        void onInitialize() override;

        void onSync() override;

        void load(MaterialData &data, PrimitiveComponent &component);

        void uploadMaterialData();

        void updateMeshMaterials();
    };
} // Metal

#endif //MATERIALSERVICE_H
