
#ifndef METAL_ENGINE_METHODFIELD_H
#define METAL_ENGINE_METHODFIELD_H

#include "../AbstractFormFieldPanel.h"
#include "../../../../../common/InspectedMethod.h"


namespace Metal {

    class MethodField : public AbstractFormFieldPanel {
        InspectedMethod &method;

    public:
        explicit MethodField(InspectedMethod &method);

        void onSync() override;

        bool isVisible() const override;
    };

} // Metal

#endif
