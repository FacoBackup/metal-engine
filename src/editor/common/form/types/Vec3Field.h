#ifndef VEC3FIELD_H
#define VEC3FIELD_H

#include <glm/vec3.hpp>

#include "../../AbstractPanel.h"
#include "../../../../common/inspection/InspectedField.h"

namespace Metal {
    class Vec3Field final : public AbstractPanel {
        InspectedField<glm::vec3> &field;
        float values[3] = {0.0f, 0.0f, 0.0f};

    public:
        explicit Vec3Field(InspectedField<glm::vec3> &field);

        void onSync() override;
    };
} // Metal

#endif //VEC3FIELD_H
