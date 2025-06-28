#ifndef RESOURCEFIELD_H
#define RESOURCEFIELD_H

#include <memory>

#include "../../AbstractPanel.h"
#include "../../../../../common/inspection/InspectedField.h"
#include "imgui.h"

namespace Metal {
    struct FileEntry;

    class ResourceField final : public AbstractPanel {
        static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                                                  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                                  ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize;
        InspectedField<std::string> &field;
        std::unique_ptr<FileEntry> entry = nullptr;
        bool open = false;

    public:
        void onInitialize() override;

        void renderButton();

        void renderModal() const;

        explicit ResourceField(InspectedField<std::string> &field);

        void onSync() override;
    };
} // Metal

#endif //RESOURCEFIELD_H
