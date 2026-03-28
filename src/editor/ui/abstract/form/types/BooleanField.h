#ifndef METAL_ENGINE_BOOLEANFIELD_H
#define METAL_ENGINE_BOOLEANFIELD_H

#include "../AbstractFormFieldPanel.h"
#include "common/FieldMetadata.h"

namespace Metal {
    class BooleanField final : public AbstractFormFieldPanel {
        FieldMetadata &field;
    public:
        explicit BooleanField(FieldMetadata &field);

        void onSync() override;

        bool isVisible() const override;
    };

} // Metal

#endif
