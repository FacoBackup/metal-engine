#ifndef RESOURCEFIELD_H
#define RESOURCEFIELD_H

#include <memory>

#include "../AbstractFormFieldPanel.h"
#include "../../../../common/InspectedField.h"
#include "../../../dto/FSEntry.h"
#include "imgui.h"

namespace Metal {
    class FilesService;
    struct FSEntry;

    class ResourceField final : public AbstractFormFieldPanel {
        InspectedField<std::string> &field;
        std::shared_ptr<FSEntry> entry = nullptr;
        mutable bool open = false;
        FilesService *filesService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {{"FilesService", &filesService}};
        }

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
