#ifndef METAL_ENGINE_INTFIELD_H
#define METAL_ENGINE_INTFIELD_H

#include "../AbstractFormFieldPanel.h"
#include "common/InspectedField.h"

namespace Metal {
    class IntField final : public AbstractFormFieldPanel {
        InspectedField<int> &field;

    public:
        explicit IntField(InspectedField<int> &field);

        void onSync() override;

        bool isVisible() const override;
    };
} // Metal

#endif
