#ifndef METAL_ENGINE_BOOLEANFIELD_H
#define METAL_ENGINE_BOOLEANFIELD_H

#include "../../AbstractPanel.h"

namespace Metal {

    class BooleanField : public AbstractPanel {
        InspectedField<bool> &field;
    public:
        explicit BooleanField(InspectedField<bool> &field);

        void onSync() override;
    };

} // Metal

#endif
