#ifndef TRANSFORMSERVICE_H
#define TRANSFORMSERVICE_H
#include <string>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "../engine-definitions.h"
#include "../../common/interface/AbstractRuntimeComponent.h"

namespace Metal {
    struct TransformComponent;

    class TransformService final : AbstractRuntimeComponent {
        glm::vec3 distanceAux{};
        glm::mat4x4 auxMat4{};
        glm::vec3 translation{};
        glm::mat4x4 auxMat42{};

    public:
        explicit TransformService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        void onSync() override;

        void traverse(EntityID entityId, bool parentHasChanged);

        void transform(TransformComponent *st, const TransformComponent *parentTransform);

        [[nodiscard]] TransformComponent *findParent(EntityID id) const;

        float getDistanceFromCamera(glm::vec3 &translation);
    };
} // Metal

#endif
