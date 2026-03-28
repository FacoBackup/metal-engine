#ifndef STRINGFIELD_H
#define STRINGFIELD_H

#include "../AbstractFormFieldPanel.h"
#include "common/FieldMetadata.h"

namespace Metal {
    class StringField final : public AbstractFormFieldPanel {
        char buffer[512];
        FieldMetadata &field;

    public:
        explicit StringField(FieldMetadata &field);

        void onSync() override;

        bool isVisible() const override;
    };
} // Metal

#endif //STRINGFIELD_H
