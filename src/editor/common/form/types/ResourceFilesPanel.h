#ifndef RESOURCEFILESPANEL_H
#define RESOURCEFILESPANEL_H
#include <functional>

#include "../../../ui/files/FilesPanel.h"

namespace Metal {
    class ResourceFilesPanel final : public FilesPanel {
        std::function<void (FileEntry *)> callback;

    public:
        explicit ResourceFilesPanel(const std::function<void (FileEntry *)> &callback) : callback(callback) {
        }

        std::function<void()> onAction() override;

        std::string getActionLabel() override;

        void openResource(FileEntry *root) override;
    };
} // Metal

#endif //RESOURCEFILESPANEL_H
