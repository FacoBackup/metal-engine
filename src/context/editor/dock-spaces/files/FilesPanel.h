#ifndef FILESPANEL_H
#define FILESPANEL_H

#include <functional>
#include <map>

#include "FilesContext.h"
#include "../docks/AbstractDockPanel.h"
#include "../../../../dto/file/ImportSettingsDTO.h"

namespace Metal {
    struct FSEntry;
    class FilePreviewPanel;
    class FormPanel;

    class FilesPanel : public AbstractDockPanel {
    protected:
        FilesContext filesContext{nullptr};
        FilePreviewPanel *previewPanel = nullptr;
        AbstractPanel *filesHeader = nullptr;
        AbstractPanel *filesListPanel = nullptr;
        float previewWidth = 200.0f;

    public:
        virtual bool renderPreview() {
            return true;
        }

        virtual std::string getActionLabel();

        virtual std::function<void()> onAction();

        void onInitialize() override;

        void onSync() override;

        static void SetIconPos(const char *text);

        void pasteSelected();

        void openSelected();

        void cutSelected();

        void selectAll();

        void deleteSelected() const;

        virtual EntryType::EntryType getTypeFilter() {
            return EntryType::NONE;
        }

        virtual void openResource(FSEntry *root);

        virtual bool renderHeader() { return true; }
    };
} // Metal

#endif //FILESPANEL_H
