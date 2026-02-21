#ifndef RESOURCEFILESPANEL_H
#define RESOURCEFILESPANEL_H
#include <functional>

#include "../../../dock-spaces/files/FilesPanel.h"

namespace Metal {
    class ResourceFilesPanel final : public FilesPanel {
        std::function<void (FileEntry *)> callback;
        EntryType::EntryType typeFilter;

    public:
        explicit ResourceFilesPanel(const std::function<void (FileEntry *)> &callback,
                                    EntryType::EntryType type) : callback(callback), typeFilter(type) {
        }

        bool renderPreview() override {
            return false;
        }

        EntryType::EntryType getTypeFilter() override {
            return typeFilter;
        }

        std::function<void()> onAction() override;

        std::string getActionLabel() override;

        bool renderHeader() override {
            return false;
        }

        void openResource(FileEntry *root) override;
    };
} // Metal

#endif //RESOURCEFILESPANEL_H
