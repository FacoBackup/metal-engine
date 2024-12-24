#ifndef METAL_ENGINE_INTFIELD_H
#define METAL_ENGINE_INTFIELD_H

#include "../../AbstractPanel.h"
#include "../../../../../common/inspection/InspectedField.h"

namespace Metal {
    class IntField final : public AbstractPanel {
        InspectedField<int> &field;

    public:
        explicit IntField(InspectedField<int> &field);

        void onSync() override;
    };
} // Metal

#endif
