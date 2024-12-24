#ifndef VEC4FIELD_H
#define VEC4FIELD_H
#include <glm/vec4.hpp>

#include "../../AbstractPanel.h"
#include "../../../../../common/inspection/InspectedField.h"

namespace Metal {
    class Vec4Field final : public AbstractPanel {
        InspectedField<glm::vec4> &field;
        float values[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    public:
        explicit Vec4Field(InspectedField<glm::vec4> &field);

        void onSync() override;
    };
} // Metal

#endif //VEC4FIELD_H
