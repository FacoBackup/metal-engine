#ifndef VOLUMESERVICE_H
#define VOLUMESERVICE_H
#include <vector>

#include "../../common/AbstractRuntimeComponent.h"
#include "../../dto/buffers/VolumeInstance.h"

namespace Metal {
    class VolumeService final : public AbstractRuntimeComponent {
        std::vector<VolumeInstance> items{};
        unsigned int count = 0;

        void registerVolumes();
    public:
        explicit VolumeService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        void update();

        [[nodiscard]] unsigned int getVolumeCount() const {
            return count;
        }
    };
} // Metal

#endif //VOLUMESERVICE_H
