#ifndef RESOURCEFIELD_H
#define RESOURCEFIELD_H

#include <memory>

#include "../AbstractFormFieldPanel.h"
#include "common/InspectedField.h"
#include "editor/dto/FSEntry.h"
#include "imgui.h"

namespace Metal {
    class FilesService;
    struct FSEntry;

    class ResourceField final : public AbstractFormFieldPanel {
        InspectedField<std::string> &field;
        std::shared_ptr<FSEntry> entry = nullptr;
        FilesService *filesService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {{"FilesService", &filesService}};
        }

        void onInitialize() override;

        void renderButton();

        explicit ResourceField(InspectedField<std::string> &field);

        void onSync() override;

        bool isVisible() const override;
    };
} // Metal

#endif //RESOURCEFIELD_H
