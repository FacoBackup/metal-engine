#ifndef FILESHEADER_H
#define FILESHEADER_H
#include <functional>

#include "FilesContext.h"
#include "../../abstract/AbstractPanel.h"

namespace Metal {
    class FilesService;

    class FilesHeaderPanel final : public AbstractPanel {
        FilesContext &filesContext;
        std::string actionLabel;
        std::function<void()> action;
        FilesService *filesService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {{"FilesService", filesService}};
        }

        explicit FilesHeaderPanel(FilesContext &files_context, const std::string &actionLabel,
                                  std::function<void()> action)
            : filesContext(files_context), actionLabel(actionLabel), action(std::move(action)) {
        }

        void onSync() override;
    };
}

#endif
