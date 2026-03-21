#ifndef VOLUME_SERVICE_H
#define VOLUME_SERVICE_H
#include <vector>

#include "../../common/IService.h"
#include "../../common/ISync.h"
#include "../../common/IEventMember.h"
#include "../dto/VolumeData.h"
#include "common/IInit.h"
#include "ApplicationEventContext.h"

namespace Metal {
    class ApplicationEventContext;
    struct WorldRepository;
    class EngineContext;

    class VolumeService final : public IService, public IEventMember, public IInit, public ISync {
        WorldRepository *worldRepository = nullptr;
        EngineContext *engineContext = nullptr;
        bool needsUpdate = true;
        std::vector<VolumeData> items{};

        void registerVolumes();

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"WorldRepository", &worldRepository},
                {"EngineContext", &engineContext}
            };
        }

        void onInitialize() override;

        void onSync() override;

        unsigned int getCount() const {
            return items.size();
        }
    };
} // Metal

#endif //LIGHTSSERVICE_H
