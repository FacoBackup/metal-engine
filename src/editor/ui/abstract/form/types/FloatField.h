#ifndef METAL_ENGINE_FLOATFIELD_H
#define METAL_ENGINE_FLOATFIELD_H

#include "../AbstractFormFieldPanel.h"
#include "common/FieldMetadata.h"

namespace Metal {

    class FloatField final : public AbstractFormFieldPanel {
        FieldMetadata &field;
    public:

        explicit FloatField(FieldMetadata &field);

        void onSync() override;

        bool isVisible() const override;
    };

} // Metal

#endif
