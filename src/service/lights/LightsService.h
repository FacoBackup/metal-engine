#ifndef LIGHTSSERVICE_H
#define LIGHTSSERVICE_H
#include <vector>

#include "../../common/AbstractRuntimeComponent.h"
#include "../../dto/buffers/LightData.h"

namespace Metal {
    class LightsService final : public AbstractRuntimeComponent {
        std::vector<LightData> items{};
        unsigned int count = 0;
        glm::vec3 sunColor{};
        glm::vec3 sunPosition{};

        void registerLights();

        void registerSun();

        static glm::vec3 CalculateSunColor(float elevation, glm::vec3 &nightColor, glm::vec3 &dawnColor,
                                           glm::vec3 &middayColor);

        static glm::vec3 BlendColors(glm::vec3 &c1, glm::vec3 &c2, float t);

    public:
        explicit LightsService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        void update();

        [[nodiscard]] unsigned int getLightVolumeCount() const {
            return count;
        }

        void computeSunInfo();

        [[nodiscard]] glm::vec3 getSunPosition() const {
            return sunPosition;
        }

        [[nodiscard]] glm::vec3 getSunColor() const {
            return sunColor;
        }
    };
} // Metal

#endif //LIGHTSSERVICE_H
