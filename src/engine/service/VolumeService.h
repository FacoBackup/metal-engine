#ifndef VOLUME_SERVICE_H
#define VOLUME_SERVICE_H
#include <vector>

#include "../../../common/AbstractRuntimeComponent.h"
#include "../dto/VolumeData.h"

namespace Metal {

    class VolumeService final : public AbstractRuntimeComponent {
        std::vector<VolumeData> items{};

        void registerVolumes();
    public:

        void onSync() override;

        unsigned int getCount() const {
            return items.size();
        }
    };
} // Metal

#endif //LIGHTSSERVICE_H
