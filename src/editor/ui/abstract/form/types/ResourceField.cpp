#include "ResourceField.h"
#include <algorithm>
#include <imgui.h>
#include <iostream>

#include "common/FileDialogUtil.h"
#include <memory>
#include "common/Icons.h"
#include "editor/ui/UIUtil.h"
#include "editor/dto/FSEntry.h"
#include "ApplicationContext.h"
#include "common/Reflection.h"
#include "editor/service/FilesService.h"
#include "editor/service/HistoryService.h"
#include "ApplicationEventContext.h"
#include "common/FileExtensions.h"
#include "editor/dto/FieldModificationEvent.h"

namespace Metal {
    constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking |
                                       ImGuiWindowFlags_NoSavedSettings;

    ResourceField::ResourceField(FieldMetadata &field) : field(field) {
    }

    void ResourceField::onInitialize() {
    }

    void ResourceField::renderButton() {
        std::string *ptr = static_cast<std::string *>(field.pointer);
        ImGui::Text(field.name.c_str());
        ImGui::BeginChild(id.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 32),
                          ImGuiChildFlags_Border);
        if (UIUtil::ButtonSimple(Icons::file_open + id, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            std::vector<nfdu8filteritem_t> filters;
            for (const auto &type: field.supportedFileTypes) {
                filters.push_back({type->name.c_str(), type->typeLabel.c_str()});
            }
            std::string selected = FileDialogUtil::PickFile(filters);
            if (!selected.empty()) {
                std::string oldValue = *ptr;
                *ptr = selected;
                historyService->recordChange(&field, oldValue);
                ApplicationEventContext::dispatch(field.instance->getClassName(),
                                                  std::make_shared<FieldModificationPayload>(field));
            }
        }
        ImGui::SameLine();
        if (entry != nullptr && UIUtil::ButtonSimple(
                Icons::close + id,
                UIUtil::ONLY_ICON_BUTTON_SIZE,
                UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            std::string oldValue = *ptr;
            entry = nullptr;
            *ptr = "";


            historyService->recordChange(&field, oldValue);
            ApplicationEventContext::dispatch(field.instance->getClassName(),
                                              std::make_shared<FieldModificationPayload>(field));
        }
        ImGui::SameLine();
        if (entry != nullptr) {
            ImGui::Text((Icons::view_in_ar + entry->name).c_str());
        } else {
            ImGui::TextDisabled((Icons::view_in_ar + "(None)").c_str());
        }
        ImGui::EndChild();
    }

    void ResourceField::onSync() {
        std::string *ptr = static_cast<std::string *>(field.pointer);
        if (ptr->size() > 0 && (entry == nullptr || entry->absolutePath != *ptr)) {
            entry = filesService->getResource(*ptr);
        }
        if (!field.disabled) {
            renderButton();
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
