#ifndef FORMPANEL_H
#define FORMPANEL_H
#include <unordered_map>

#include "AccordionPanel.h"
#include "../AbstractPanel.h"

namespace Metal {
    class Inspectable;

    class FormPanel final : public AbstractPanel {
        Inspectable *inspection = nullptr;

    public:
        void processFields(std::unordered_map<std::string, AccordionPanel *> &pairs);

        void setInspection(Inspectable *inspection);

        void onSync() override;
    };
}

#endif
