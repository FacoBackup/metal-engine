#include "ResourceField.h"
#include <algorithm>
#include <imgui.h>
#include <iostream>

#include "ResourceFilesPanel.h"
#include <memory>
#include "common/Icons.h"
#include "editor/ui/UIUtil.h"
#include "editor/dto/FSEntry.h"
#include "ApplicationContext.h"
#include "common/Inspectable.h"
#include "editor/service/FilesService.h"
#include "editor/service/HistoryService.h"
#include "ApplicationEventContext.h"
#include "editor/dto/FieldModificationEvent.h"

namespace Metal {
    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking |
                                       ImGuiWindowFlags_NoSavedSettings;

    ResourceField::ResourceField(InspectedField<std::string> &field) : field(field) {
    }

    void ResourceField::onInitialize() {
        initializePanel<ResourceFilesPanel>(true, [this](const std::shared_ptr<FSEntry> &file) {
            if (file == nullptr) {
                open = false;
                return;
            }
            std::string oldValue = *field.field;
            *field.field = file->absolutePath;

            historyService->recordChange(&field, oldValue);
            ApplicationEventContext::dispatch(field.instance->getClassName(), std::make_shared<FieldModificationPayload>(field));
            open = false;
        }, field.supportedFileTypes);
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
            std::string oldValue = *field.field;
            entry = nullptr;
            *field.field = "";


            historyService->recordChange(&field, oldValue);
            ApplicationEventContext::dispatch(field.instance->getClassName(), std::make_shared<FieldModificationPayload>(field));
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
        ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x - size.x,
                                       ImGui::GetWindowPos().y), ImGuiCond_FirstUseEver);
        if (ImGui::Begin(field.nameWithId.c_str(), &open, flags)) {
            onSyncChildren();
        }
        ImGui::End();
    }

    void ResourceField::onSyncChildren() const {
        AbstractPanel::onSyncChildren();
    }

    void ResourceField::onSync() {
        if (field.field->size() > 0 && (entry == nullptr || entry->absolutePath != *field.field)) {
            entry = filesService->getResource(*field.field);
        }
        if (!field.disabled) {
            renderButton();
            renderModal();
        } else {
            ImGui::Text("%s: %s", field.name.c_str(), (entry != nullptr ? entry->name.c_str() : "(None)"));
        }
    }

    bool ResourceField::isVisible() const {
        if (!filter || filter->empty()) return true;
        std::string lowerName = field.name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
        return lowerName.find(*filter) != std::string::npos;
    }
} // Metal
