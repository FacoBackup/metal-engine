#ifndef LIGHTSSERVICE_H
#define LIGHTSSERVICE_H
#include <vector>

#include "../../common/AbstractRuntimeComponent.h"
#include "../../dto/buffers/LightData.h"

namespace Metal {
    class LightService final : public AbstractRuntimeComponent {
        std::vector<LightData> items{};

        void registerLights();

        void registerSun();

    public:
        void onSync() override;

        unsigned int getCount() const {
            return items.size();
        }
    };
} // Metal

#endif //LIGHTSSERVICE_H
