#ifndef METAL_ENGINE_BOOLEANFIELD_H
#define METAL_ENGINE_BOOLEANFIELD_H

#include "../AbstractFormFieldPanel.h"
#include "../../../../../common/InspectedField.h"

namespace Metal {
    class BooleanField final : public AbstractFormFieldPanel {
        InspectedField<bool> &field;
    public:
        explicit BooleanField(InspectedField<bool> &field);

        void onSync() override;

        bool isVisible() const override;
    };

} // Metal

#endif
