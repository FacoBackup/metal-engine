#include "VolumeService.h"
#include "../../MetalContextProvider.h"
#include "../data/BufferInstance.h"

namespace Metal {
    void VolumeService::registerVolumes() {
        for (auto &entry: SINGLETONS.worldRepository.volumes) {
            if (SINGLETONS.worldRepository.hiddenEntities.contains(entry.first)) {
                continue;
            }
            auto &t = SINGLETONS.worldRepository.transforms.at(entry.first);
            auto &translation = t.translation;
            auto &l = entry.second;

            items.push_back(VolumeBuffer(
                glm::vec4(l.albedo, l.samples),
                translation,
                t.scale,
                l.density,
                l.scatteringAlbedo,
                l.g
            ));
        }
    }

    void VolumeService::update() {
        items.clear();
        registerVolumes();

        if (!items.empty()) {
            SINGLETONS.coreBuffers.volumesBuffer->update(items.data());
        }
        count = items.size();
    }
} // Metal
