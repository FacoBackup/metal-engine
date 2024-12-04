#ifndef METAL_ENGINE_FLOATFIELD_H
#define METAL_ENGINE_FLOATFIELD_H

#include "../../AbstractPanel.h"


namespace Metal {

    class FloatField : public AbstractPanel {
        InspectedField<float> &field;
    public:

        explicit FloatField(InspectedField<float> &field);

        void onSync() override;

    };

} // Metal

#endif
