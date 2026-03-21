#ifndef FORMPANEL_H
#define FORMPANEL_H
#include <unordered_map>

#include "../AbstractPanel.h"
#include <string>

namespace Metal {
    class Inspectable;

    class HistoryService;

    class ThemeService;

    class FormPanel final : public AbstractPanel {
        std::unordered_map<std::string, Inspectable *> inspectionMap{};
        std::string searchFilter;
        char searchBuffer[256] = "";

        HistoryService *historyService = nullptr;
        ThemeService *themeService = nullptr;

        void processFields(Inspectable *inspection);

    public:
        void setInspection(Inspectable *inspection);

        void onSync() override;

        void resetForm();

        std::vector<Dependency> getDependencies() override {
            return {
                {"HistoryService", &historyService},
                {"ThemeService", &themeService}
            };
        }
    };
}

#endif
