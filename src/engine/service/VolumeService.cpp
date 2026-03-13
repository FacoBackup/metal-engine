#include "VolumeService.h"
#include "../../ApplicationContext.h"
#include "../dto/VolumeComponent.h"
#include "../resource/BufferInstance.h"
#include "../../editor/enum/EngineResourceIDs.h"
#include "../repository/WorldRepository.h"
#include "../EngineContext.h"

namespace Metal {
    void VolumeService::registerVolumes() {
        auto view = worldRepository.registry.view<VolumeComponent, TransformComponent>();
        for (auto [entityId, l, t]: view.each()) {
            if (worldRepository.hiddenEntities.contains(entityId)) {
                continue;
            }

            auto &translation = t.translation;

            items.push_back(VolumeData(
                glm::vec4(l.albedo, l.samples),
                translation,
                t.scale,
                glm::vec3(l.density, l.scatteringAlbedo, l.g)
            ));
        }
    }

    // TODO - ADD EVENT SYSTEM THAT TRIGGERS THIS UPDATE
    void VolumeService::onSync() {
        items.clear();

        registerVolumes();

        if (!items.empty()) {
            engineContext.currentFrame->getResourceAs<BufferInstance>(RID_VOLUMES_BUFFER)->update(items.data());
        }
    }
} // Metal
