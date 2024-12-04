#ifndef METAL_ENGINE_INTFIELD_H
#define METAL_ENGINE_INTFIELD_H

#include "../../AbstractPanel.h"

namespace Metal {

    class IntField : public AbstractPanel {
        InspectedField<int> &field;
    public:
        explicit IntField(InspectedField<int> &field);

        void onSync() override;
    };

} // Metal

#endif
