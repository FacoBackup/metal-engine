#ifndef FILESHEADER_H
#define FILESHEADER_H
#include <functional>

#include "FilesContext.h"
#include "../../common/AbstractPanel.h"

namespace Metal {
    class FilesHeader final : public AbstractPanel {
        FilesContext &filesContext;
        std::string actionLabel;
        std::function<void()> action;
        int editorMode = 0;
    public:
        explicit FilesHeader(FilesContext &files_context, const std::string &actionLabel,
                             std::function<void()> action)
            : filesContext(files_context), actionLabel(actionLabel), action(std::move(action)) {
        }

        void renderOptions() const;

        void onSync() override;
    };
}

#endif
