#include "VolumeService.h"
#include "../../context/ApplicationContext.h"
#include "../buffer/BufferInstance.h"

namespace Metal {
    void VolumeService::registerVolumes() {
        for (auto &entry: context.worldRepository.volumes) {
            if (context.worldRepository.hiddenEntities.contains(entry.first)) {
                continue;
            }
            auto &t = context.worldRepository.transforms.at(entry.first);
            auto &translation = t.translation;
            auto &l = entry.second;

            items.push_back(VolumeInstance(
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
            context.coreBuffers.volumesBuffer->update(items.data());
        }
        count = items.size();
    }
} // Metal
