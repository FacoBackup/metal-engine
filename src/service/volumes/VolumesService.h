#ifndef VOLUMESSERVICE_H
#define VOLUMESSERVICE_H
#include <vector>

#include "../../common/AbstractRuntimeComponent.h"
#include "../../dto/buffers/VolumeData.h"

namespace Metal {
    class VolumesService final : public AbstractRuntimeComponent {
        std::vector<VolumeData> volumes{};
        unsigned int volumeCount = 0;

    public:
        explicit VolumesService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        void updateVolumes();

        [[nodiscard]] unsigned int getVolumeCount() const {
            return volumeCount;
        }
    };
} // Metal

#endif //VOLUMESSERVICE_H
