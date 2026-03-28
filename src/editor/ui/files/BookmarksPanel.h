#ifndef BOOKMARKSPANEL_H
#define BOOKMARKSPANEL_H

#include "editor/ui/abstract/AbstractPanel.h"
#include "FilesContext.h"

namespace Metal {
    struct EditorRepository;
    class FilesService;
    class ThemeService;

    class BookmarksPanel final : public AbstractPanel {
        FilesContext &filesContext;
        EditorRepository *editorRepository = nullptr;
        FilesService *filesService = nullptr;
        ThemeService *themeService = nullptr;
        bool isOpen = false;

    public:
        explicit BookmarksPanel(FilesContext &filesContext) : filesContext(filesContext) {}

        std::vector<Dependency> getDependencies() override {
            return {
                {"EditorRepository", &editorRepository},
                {"FilesService", &filesService},
                {"ThemeService", &themeService}
            };
        }

        void onSync() override;
    };
}

#endif //BOOKMARKSPANEL_H
