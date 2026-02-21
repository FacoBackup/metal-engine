#ifndef RESOURCEFIELD_H
#define RESOURCEFIELD_H

#include <memory>

#include "../../AbstractPanel.h"
#include "../../../../../common/inspection/InspectedField.h"
#include "imgui.h"

namespace Metal {
    struct FileEntry;

    class ResourceField final : public AbstractPanel {
        InspectedField<std::string> &field;
        std::unique_ptr<FileEntry> entry = nullptr;
        mutable bool open = false;

    public:
        void onInitialize() override;

        void renderButton();

        void renderModal() const;

        explicit ResourceField(InspectedField<std::string> &field);

        void onSync() override;
    };
} // Metal

#endif //RESOURCEFIELD_H
