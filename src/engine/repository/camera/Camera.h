#ifndef CAMERA_H
#define CAMERA_H
#include <glm/vec3.hpp>

#include "../../../common/inspection/Inspectable.h"
#include "../../../common/util/Util.h"

namespace Metal {
    class Camera final : public Inspectable {
    public:
        glm::vec3 position{};
        float pitch = 0.f;
        float yaw = 0.f;
        bool isOrthographic = false;
        float zFar = 10000;
        float zNear = .1f;
        float fov = 90 * Util::TO_RADIANS;
        float aspectRatio = 1;
        float orthographicProjectionSize = 50;
    };
}

#endif
