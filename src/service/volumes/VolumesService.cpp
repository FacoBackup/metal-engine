#include "VolumesService.h"
#include "../../context/ApplicationContext.h"
#include "../buffer/BufferInstance.h"

namespace Metal {
    void VolumesService::updateVolumes() {
        int index = 0;

        volumes.clear();
        for (auto &entry: context.worldRepository.volumes) {
            if (context.worldRepository.hiddenEntities.contains(entry.first)) {
                continue;
            }
            auto &t = context.worldRepository.transforms.at(entry.first);
            auto &translation = t.translation;
            auto &l = entry.second;

            volumes[index] = VolumeData(
                l.albedo,
                translation,
                t.scale,
                l.density
            );
            index++;
        }

        if (!volumes.empty()) {
            context.coreBuffers.volumes->update(volumes.data());
        }
        volumeCount = index;
    }
} // Metal
