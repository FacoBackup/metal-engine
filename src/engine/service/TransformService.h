#ifndef TRANSFORMSERVICE_H
#define TRANSFORMSERVICE_H
#include <string>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "../../editor/enum/engine-definitions.h"
#include "../../common/AbstractRuntimeComponent.h"

namespace Metal {
    struct TransformComponent;

    class TransformService final : AbstractRuntimeComponent {
        glm::vec3 distanceAux{};
        glm::mat4x4 auxMat4{};
        glm::vec3 translation{};
        glm::mat4x4 auxMat42{};

    public:

        void onSync() override;

        void transform(TransformComponent *st, const TransformComponent *parentTransform);
    };
} // Metal

#endif
