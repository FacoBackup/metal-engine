#ifndef METAL_ENGINE_INTFIELD_H
#define METAL_ENGINE_INTFIELD_H

#include "../AbstractFormFieldPanel.h"
#include "common/FieldMetadata.h"

namespace Metal {
    class IntField final : public AbstractFormFieldPanel {
        FieldMetadata &field;

    public:
        explicit IntField(FieldMetadata &field);

        void onSync() override;

        bool isVisible() const override;
    };
} // Metal

#endif
