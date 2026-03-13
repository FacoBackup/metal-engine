#ifndef VOLUME_SERVICE_H
#define VOLUME_SERVICE_H
#include <vector>

#include "../../common/AbstractRuntimeComponent.h"
#include "../dto/VolumeData.h"

namespace Metal {

    struct WorldRepository;
    class EngineContext;

    class VolumeService final : public AbstractRuntimeComponent {
        WorldRepository &worldRepository;
        EngineContext &engineContext;

        std::vector<VolumeData> items{};

        void registerVolumes();
    public:
        explicit VolumeService(WorldRepository &worldRepository, EngineContext &engineContext)
            : worldRepository(worldRepository), engineContext(engineContext) {}
        VolumeService() = delete;

        void onSync() override;

        unsigned int getCount() const {
            return items.size();
        }
    };
} // Metal

#endif //LIGHTSSERVICE_H
