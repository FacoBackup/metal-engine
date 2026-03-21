#ifndef SYSTEMDIRPANEL_H
#define SYSTEMDIRPANEL_H

#include "../../abstract/AbstractPanel.h"
#include "FilesContext.h"
#include <string>
#include <vector>

namespace Metal {
    class FilesService;
    class ThemeService;

    class SystemDirPanel final : public AbstractPanel {
        FilesContext &filesContext;
        FilesService *filesService = nullptr;
        ThemeService *themeService = nullptr;
        bool isOpen = false;

        struct SystemDir {
            std::string name;
            std::string path;
            std::string icon;
        };
        std::vector<SystemDir> systemDirs;

    public:
        explicit SystemDirPanel(FilesContext &filesContext) : filesContext(filesContext) {}

        std::vector<Dependency> getDependencies() override {
            return {
                {"FilesService", &filesService},
                {"ThemeService", &themeService}
            };
        }

        void onInitialize() override;
        void onSync() override;
    };
}

#endif //SYSTEMDIRPANEL_H
