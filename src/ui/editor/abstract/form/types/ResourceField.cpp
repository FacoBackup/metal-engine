#include "ResourceField.h"

#include <imgui.h>
#include <iostream>

#include "ResourceFilesPanel.h"
#include "../../../../../common/enum/Icons.h"
#include "../../../../util/UIUtil.h"
#include "../../../../../common/FileEntry.h"
#include "../../../../../MetalContextProvider.h"
#include "../../../../../common/inspection/Inspectable.h"

namespace Metal {
    ResourceField::ResourceField(InspectedField<std::string> &field) : field(field) {
    }

    void ResourceField::onInitialize() {
        appendChild(new ResourceFilesPanel([this](FileEntry *file) {
            if (file == nullptr) {
                open = false;
                return;
            }
            if (file->type == field.resourceType) {
                *field.field = file->getId();
                field.instance->registerChange();
                field.instance->onUpdate(&field);
                open = false;
            }
        }));
    }

    void ResourceField::renderButton() {
        ImGui::Text(field.name.c_str());
        ImGui::BeginChild(id.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 32),
                          ImGuiChildFlags_Border);
        if (UIUtil::ButtonSimple(Icons::file_open + id, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            open = true;
        }
        ImGui::SameLine();
        if (entry != nullptr && UIUtil::ButtonSimple(
                Icons::close + id,
                UIUtil::ONLY_ICON_BUTTON_SIZE,
                UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            entry = nullptr;
            *field.field = "";
            field.instance->registerChange();
            field.instance->onUpdate(&field);
        }
        ImGui::SameLine();
        if (entry != nullptr) {
            ImGui::Text((Icons::view_in_ar + entry->name).c_str());
        } else {
            ImGui::TextDisabled((Icons::view_in_ar + "(None)").c_str());
        }
        ImGui::EndChild();
    }

    void ResourceField::renderModal() const {
        if (!open) {
            return;
        }
        ImVec2 size = ImVec2(ImGui::GetMainViewport()->Size.x / 2,
                             ImGui::GetMainViewport()->Size.y / 2);
        ImGui::SetNextWindowSize(size);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x - size.x,
                                       ImGui::GetWindowPos().y));
        if (ImGui::Begin(field.nameWithId.c_str(), nullptr, flags)) {
            onSyncChildren();
            ImGui::End();
        }
    }

    void ResourceField::onSync() {
        if (field.field->size() > 0 && (entry == nullptr || entry->getId() != *field.field)) {
            entry = SINGLETONS.filesService.getResource(*field.field);
        }
        if (!field.disabled) {
            renderButton();
            renderModal();
        } else {
            ImGui::Text("%s: %s", field.name.c_str(), (entry != nullptr ? entry->name.c_str() : "(None)"));
        }
    }
} // Metal
