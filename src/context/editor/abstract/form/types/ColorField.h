#ifndef COLORFIELD_H
#define COLORFIELD_H
#include <glm/vec3.hpp>

#include "../AbstractFormFieldPanel.h"
#include "../../../../../common/inspection/InspectedField.h"

namespace Metal {
    class ColorField final : public AbstractFormFieldPanel {
        InspectedField<glm::vec3> &field;
        float values[3] = {0.0f, 0.0f, 0.0f};

    public:
        explicit ColorField(InspectedField<glm::vec3> &field);

        void onSync() override;

        bool isVisible() const override;
    };
} // Metal

#endif //COLORFIELD_H
