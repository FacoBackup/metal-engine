#ifndef RESOURCEFIELD_H
#define RESOURCEFIELD_H

#include <memory>

#include "../AbstractFormFieldPanel.h"
#include "../../../../../common/inspection/InspectedField.h"
#include "imgui.h"

namespace Metal {
    struct FileEntry;

    class ResourceField final : public AbstractFormFieldPanel {
        InspectedField<std::string> &field;
        std::unique_ptr<FileEntry> entry = nullptr;
        mutable bool open = false;

    public:
        void onInitialize() override;

        void renderButton();

        void renderModal() const;

        void onSyncChildren() const override;

        explicit ResourceField(InspectedField<std::string> &field);

        void onSync() override;

        bool isVisible() const override;
    };
} // Metal

#endif //RESOURCEFIELD_H
