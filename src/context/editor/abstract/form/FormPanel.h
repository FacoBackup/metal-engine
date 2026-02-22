#ifndef FORMPANEL_H
#define FORMPANEL_H
#include <unordered_map>

#include "AccordionPanel.h"
#include "../AbstractPanel.h"

namespace Metal {
    class Inspectable;

    class FormPanel final : public AbstractPanel {
        std::unordered_map<std::string, Inspectable*> inspectionMap{};

    public:
        void processFields(Inspectable * inspection);

        void setInspection(Inspectable *inspection);

        void onSync() override;

        void resetForm();
    };
}

#endif
