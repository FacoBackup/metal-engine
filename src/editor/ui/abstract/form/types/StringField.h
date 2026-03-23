#ifndef STRINGFIELD_H
#define STRINGFIELD_H

#include "../AbstractFormFieldPanel.h"
#include "common/InspectedField.h"


namespace Metal {
    class StringField final : public AbstractFormFieldPanel {
        char buffer[512];
        InspectedField<std::string> &field;

    public:
        explicit StringField(InspectedField<std::string> &field);

        void onSync() override;

        bool isVisible() const override;
    };
} // Metal

#endif //STRINGFIELD_H
