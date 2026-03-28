#ifndef QUATFIELD_H
#define QUATFIELD_H
#include <glm/glm.hpp>
#include "../AbstractFormFieldPanel.h"
#include "common/FieldMetadata.h"

namespace Metal {
    class QuatField final : public AbstractFormFieldPanel {
        FieldMetadata &field;
        float values[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        glm::vec3 eulerValues = {0, 0, 0};
        bool isEditing = false;

    public:
        explicit QuatField(FieldMetadata &field);

        void onSync() override;

        bool isVisible() const override;
    };
} // Metal

#endif //QUATFIELD_H
