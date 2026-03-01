#include "VolumeService.h"
#include "../../context/ApplicationContext.h"
#include "../../repository/world/components/VolumeComponent.h"
#include "../buffer/BufferInstance.h"

namespace Metal {
    void VolumeService::registerVolumes() {
        auto view = CTX.worldRepository.registry.view<VolumeComponent, TransformComponent>();
        for (auto [entity, l, t]: view.each()) {
            const auto entityId = static_cast<EntityID>(entity);
            if (CTX.worldRepository.hiddenEntities.contains(entityId)) {
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

    void VolumeService::onSync() {
        items.clear();

        registerVolumes();

        if (!items.empty()) {
            CTX.bufferService.getResource("volumesBuffer")->update(items.data());
        }
    }
} // Metal
