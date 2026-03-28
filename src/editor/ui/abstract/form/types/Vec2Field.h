#ifndef VEC2FIELD_H
#define VEC2FIELD_H

#include <glm/vec2.hpp>

#include "../AbstractFormFieldPanel.h"
#include "common/FieldMetadata.h"

namespace Metal {
    class Vec2Field final : public AbstractFormFieldPanel {
        FieldMetadata &field;
        float values[2] = {0, 0};

    public:
        explicit Vec2Field(FieldMetadata &field);

        void onSync() override;

        bool isVisible() const override;
    };
} // Metal

#endif //VEC2FIELD_H
