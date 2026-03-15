#ifndef VEC3FIELD_H
#define VEC3FIELD_H

#include <glm/vec3.hpp>

#include "../AbstractFormFieldPanel.h"
#include "../../../../common/InspectedField.h"


namespace Metal {
    class Vec3Field final : public AbstractFormFieldPanel {
        InspectedField<glm::vec3> &field;
        float values[3] = {0.0f, 0.0f, 0.0f};

    public:
        explicit Vec3Field(InspectedField<glm::vec3> &field);

        void onSync() override;

        bool isVisible() const override;
    };
} // Metal

#endif //VEC3FIELD_H
