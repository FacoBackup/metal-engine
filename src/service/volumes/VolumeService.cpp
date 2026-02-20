#include "VolumeService.h"
#include "../../context/ApplicationContext.h"
#include "../buffer/BufferInstance.h"

namespace Metal {
    void VolumeService::registerVolumes() {
        for (auto &entry: CTX.worldRepository.volumes) {
            if (CTX.worldRepository.hiddenEntities.contains(entry.first)) {
                continue;
            }

            auto &t = CTX.worldRepository.transforms.at(entry.first);
            auto &translation = t.translation;
            auto &l = entry.second;

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
            CTX.coreBuffers.volumesBuffer->update(items.data());
        }
    }
} // Metal
