#ifndef LIGHTSSERVICE_H
#define LIGHTSSERVICE_H
#include <vector>

#include "../../common/AbstractRuntimeComponent.h"
#include "../../dto/buffers/LightData.h"

namespace Metal {
    class LightService final : public AbstractRuntimeComponent {
        std::vector<LightData> items{};
        glm::vec3 sunColor{};
        glm::vec3 sunPosition{};

        void registerLights();

        static glm::vec3 CalculateSunColor(float elevation, glm::vec3 &nightColor, glm::vec3 &dawnColor,
                                           glm::vec3 &middayColor);

        static glm::vec3 BlendColors(glm::vec3 &c1, glm::vec3 &c2, float t);

    public:
        explicit LightService()
            : AbstractRuntimeComponent() {
        }

        void onSync() override;

        void computeSunInfo();

        [[nodiscard]] glm::vec3 getSunPosition() const {
            return sunPosition;
        }

        [[nodiscard]] glm::vec3 getSunColor() const {
            return sunColor;
        }

        unsigned int getCount() const {
            return items.size();
        }
    };
} // Metal

#endif //LIGHTSSERVICE_H
