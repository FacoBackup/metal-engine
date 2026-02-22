#ifndef VEC2FIELD_H
#define VEC2FIELD_H

#include <glm/vec2.hpp>

#include "../AbstractFormFieldPanel.h"
#include "../../../../../common/inspection/InspectedField.h"

namespace Metal {
    class Vec2Field final : public AbstractFormFieldPanel {
        InspectedField<glm::vec2> &field;
        float values[2] = {0, 0};

    public:
        explicit Vec2Field(InspectedField<glm::vec2> &field);

        void onSync() override;

        bool isVisible() const override;
    };
} // Metal

#endif //VEC2FIELD_H
