#ifndef ABSTRACTDIRECTORYPANEL_H
#define ABSTRACTDIRECTORYPANEL_H

#include "editor/ui/abstract/AbstractPanel.h"
#include "FilesContext.h"
#include <vector>
#include <string>
#include <memory>

namespace Metal {
    struct FSEntry;
    class FilesService;
    class FileImporterService;

    class AbstractDirectoryPanel : public AbstractPanel {
    protected:
        FilesContext &filesContext;
        FilesService *filesService = nullptr;
        FileImporterService *fileImporterService = nullptr;

        bool canInteract(std::shared_ptr<FSEntry> entry) const;

        std::string getEntryIcon(std::shared_ptr<FSEntry> entry) const;

        bool matchesSearch(std::shared_ptr<FSEntry> entry) const;

        static std::string getExtensionLabel(const std::string &extension);

        static void renderInfoTooltip(std::shared_ptr<FSEntry> entry);

    public:
        void openResource(std::shared_ptr<FSEntry> root) const;

        std::vector<Dependency> getDependencies() override;

        explicit AbstractDirectoryPanel(FilesContext &filesContext);
    };
}

#endif //ABSTRACTDIRECTORYPANEL_H
