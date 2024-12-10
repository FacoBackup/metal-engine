#ifndef CAMERAMOVEMENTSERVICE_H
#define CAMERAMOVEMENTSERVICE_H
#include <complex>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../../../common/interface/AbstractRuntimeComponent.h"

constexpr float PI_OVER_2 = glm::pi<float>() / 2.0f;
constexpr float PI_2 = glm::pi<float>() * 2.0f;
constexpr float MIN_MAX_PITCH = glm::radians(89.0f);

namespace Metal {
    class Camera;

    class CameraMovementService final : public AbstractRuntimeComponent{
        glm::vec3 toApplyTranslation{0.0f};
        glm::vec3 xAxis{0.0f};
        glm::vec3 yAxis{0.0f};
        glm::vec3 zAxis{0.0f};

    public:
        explicit CameraMovementService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        void handleInputInternal(Camera &camera);

        void handleMouse(Camera &camera, bool isFirstMovement);

        void updateDelta(bool isFirstMovement) ;

        void handleInput(Camera &camera, bool isFirstMovement) {
            handleInputInternal(camera);
            handleMouse(camera, isFirstMovement);
        }

        void createViewMatrix(const Camera &camera);
    };
} // Metal

#endif //CAMERAMOVEMENTSERVICE_H
