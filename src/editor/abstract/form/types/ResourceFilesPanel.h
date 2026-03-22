#ifndef RESOURCEFILESPANEL_H
#define RESOURCEFILESPANEL_H
#include <functional>

#include "../../../dock-spaces/files/FilesPanel.h"

namespace Metal {
    class ResourceFilesPanel final : public FilesPanel {
        std::function<void (std::shared_ptr<FSEntry>)> callback;
        std::vector<std::string> supportedFileTypes;

    public:
        explicit ResourceFilesPanel(const std::function<void (std::shared_ptr<FSEntry>)> &callback,
                                    std::vector<std::string> supportedFileTypes)
            : callback(callback), supportedFileTypes(std::move(supportedFileTypes)) {
        }

        bool renderPreview() override {
            return false;
        }

        std::vector<std::string> getTypeFilter() override {
            return supportedFileTypes;
        }

        bool renderHeader() override {
            return false;
        }

        void openResource(std::shared_ptr<FSEntry> root) override;
    };
} // Metal

#endif //RESOURCEFILESPANEL_H
