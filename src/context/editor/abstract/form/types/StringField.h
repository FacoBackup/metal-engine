#ifndef STRINGFIELD_H
#define STRINGFIELD_H

#include "../../AbstractPanel.h"
#include "../../../../../common/inspection/InspectedField.h"


namespace Metal {
    class StringField final : public AbstractPanel {
        char buffer[512];
        InspectedField<std::string> &field;

    public:
        explicit StringField(InspectedField<std::string> &field);

        void onSync() override;
    };
} // Metal

#endif //STRINGFIELD_H
