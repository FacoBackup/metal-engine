#ifndef LIGHTSSERVICE_H
#define LIGHTSSERVICE_H
#include <vector>
#include <glm/glm.hpp>

#include "../../common/IService.h"
#include "../../common/ISync.h"
#include "../dto/LightData.h"

#include "../../common/IInit.h"
#include "ApplicationEventContext.h"
#include "common/IEventMember.h"

namespace Metal {
    class EngineContext;
    struct EngineRepository;

    class MeshService;
    struct WorldRepository;
    class ApplicationEventContext;

    class LightService final : public IService, public IInit, public ISync, public IEventMember {
        EngineContext *engineContext = nullptr;
        EngineRepository *engineRepository = nullptr;
        WorldRepository *worldRepository = nullptr;
        MeshService *meshService = nullptr;

        std::vector<LightData> items{};
        glm::vec3 sunPosition{};
        bool needsUpdate = false;

        void registerLights();


        static glm::vec3 CalculateSunColor(float elevation, const glm::vec3 &nightColor, const glm::vec3 &dawnColor,
                                           const glm::vec3 &middayColor);

        static glm::vec3 BlendColors(const glm::vec3 &c1, const glm::vec3 &c2, float t);

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"EngineContext", &engineContext},
                {"EngineRepository", &engineRepository},
                {"WorldRepository", &worldRepository},
                {"MeshService", &meshService}
            };
        }

        void onSync() override;

        void computeSunInfo();

        void onInitialize() override;

        [[nodiscard]] glm::vec3 getSunPosition() const {
            return sunPosition;
        }

        unsigned int getCount() const {
            return items.size();
        }
    };
} // Metal

#endif //LIGHTSSERVICE_H
