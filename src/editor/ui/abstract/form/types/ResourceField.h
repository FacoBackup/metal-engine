#ifndef RESOURCEFIELD_H
#define RESOURCEFIELD_H

#include <memory>

#include "../AbstractFormFieldPanel.h"
#include "common/FieldMetadata.h"
#include "editor/dto/FSEntry.h"
#include "imgui.h"

namespace Metal {
    class FilesService;
    struct FSEntry;

    class ResourceField final : public AbstractFormFieldPanel {
        FieldMetadata &field;
        std::shared_ptr<FSEntry> entry = nullptr;
        FilesService *filesService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {{"FilesService", &filesService}};
        }

        void onInitialize() override;

        void renderButton();

        explicit ResourceField(FieldMetadata &field);

        void onSync() override;

        bool isVisible() const override;
    };
} // Metal

#endif //RESOURCEFIELD_H
