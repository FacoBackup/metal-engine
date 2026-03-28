#ifndef FORMPANEL_H
#define FORMPANEL_H
#include <unordered_map>

#include "../AbstractPanel.h"
#include <string>

namespace Metal {
    class Reflection;

    class HistoryService;

    class ThemeService;

    class FormPanel final : public AbstractPanel {
        std::unordered_map<std::string, Reflection *> inspectionMap{};
        std::string searchFilter;
        char searchBuffer[256] = "";

        HistoryService *historyService = nullptr;
        ThemeService *themeService = nullptr;

        void processFields(Reflection *inspection);

    public:
        void setInspection(Reflection *inspection);

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
