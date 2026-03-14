#ifndef VOLUME_SERVICE_H
#define VOLUME_SERVICE_H
#include <vector>

#include "../../common/IService.h"
#include "../../common/ISync.h"
#include "../dto/VolumeData.h"

namespace Metal {

    struct WorldRepository;
    class EngineContext;

    class VolumeService final : public IService, public ISync {
        WorldRepository *worldRepository = nullptr;
        EngineContext *engineContext = nullptr;

        std::vector<VolumeData> items{};

        void registerVolumes();
    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"WorldRepository", worldRepository},
                {"EngineContext", engineContext}
            };
        }

        void onSync() override;

        unsigned int getCount() const {
            return items.size();
        }
    };
} // Metal

#endif //LIGHTSSERVICE_H
