#ifndef FORMPANEL_H
#define FORMPANEL_H
#include <unordered_map>

#include "../AbstractPanel.h"
#include <string>

namespace Metal {
    class Inspectable;

    class FormPanel final : public AbstractPanel {
        std::unordered_map<std::string, Inspectable*> inspectionMap{};
        std::string searchFilter;
        char searchBuffer[256] = "";

    public:
        void processFields(Inspectable * inspection);

        void setInspection(Inspectable *inspection);

        void onSync() override;

        void resetForm();
    };
}

#endif
