#ifndef RESOURCEFILESPANEL_H
#define RESOURCEFILESPANEL_H
#include <functional>

#include "../../../dock-spaces/files/FilesPanel.h"

namespace Metal {
    class ResourceFilesPanel final : public AbstractPanel {
        std::function<void (FileEntry *)> callback;

    public:
        explicit ResourceFilesPanel(const std::function<void (FileEntry *)> &callback) : callback(callback) {
        }
    };
} // Metal

#endif //RESOURCEFILESPANEL_H
