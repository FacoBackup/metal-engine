#ifndef METAL_ENGINE_FLOATFIELD_H
#define METAL_ENGINE_FLOATFIELD_H

#include "../AbstractFormFieldPanel.h"
#include "../../../../../common/inspection/InspectedField.h"


namespace Metal {

    class FloatField final : public AbstractFormFieldPanel {
        InspectedField<float> &field;
    public:

        explicit FloatField(InspectedField<float> &field);

        void onSync() override;

        bool isVisible() const override;
    };

} // Metal

#endif
