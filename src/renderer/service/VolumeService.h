#ifndef VOLUMESERVICE_H
#define VOLUMESERVICE_H
#include <vector>

#include "../../common/AbstractRuntimeComponent.h"
#include "../data/VolumeBuffer.h"

namespace Metal {
    class VolumeService final : public AbstractRuntimeComponent {
        std::vector<VolumeBuffer> items{};
        unsigned int count = 0;

        void registerVolumes();
    public:
        void update();

        [[nodiscard]] unsigned int getVolumeCount() const {
            return count;
        }
    };
} // Metal

#endif //VOLUMESERVICE_H
