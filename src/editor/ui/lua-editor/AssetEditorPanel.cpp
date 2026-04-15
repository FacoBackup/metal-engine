#include "AssetEditorPanel.h"
#include "imgui.h"
#include "common/Icons.h"
#include "editor/ui/UIUtil.h"
#include "common/FileDialogUtil.h"
#include "editor/ui/abstract/form/FormPanel.h"
#include <fstream>
#include <filesystem>
#include <chrono>
#include <format>
#include <cstring>
#include <nfd.hpp>

namespace fs = std::filesystem;

namespace Metal {
    AssetEditorPanel::AssetEditorPanel() {
        std::memset(pathInput, 0, sizeof(pathInput));
        std::memset(contentBuffer, 0, sizeof(contentBuffer));
    }

    void AssetEditorPanel::onSync() {
        ImGui::Text("%s Asset Editor", Icons::edit.c_str());
        ImGui::Separator();

        ImGui::InputText("File Path", pathInput, sizeof(pathInput), ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button((Icons::folder_open + "##load").c_str())) {
            std::vector<nfdu8filteritem_t> filters = {
                {"Lua Script", "lua"},
                {"Material", "mmat"}
            };
            std::string selected = FileDialogUtil::PickFile(filters);
            if (!selected.empty()) {
                loadFile(selected);
            }
        }
        UIUtil::RenderTooltip("Load Asset");

        if (!lastModified.empty()) {
            ImGui::Text("Last Modified: %s", lastModified.c_str());
        } else {
            ImGui::Text("No asset loaded");
        }

        ImGui::Separator();

        if (isMaterial) {
            renderMaterialEditor();
        } else if (!filePath.empty()) {
            renderScriptEditor();
        }
    }

    void AssetEditorPanel::renderScriptEditor() {
        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        contentSize.y -= ImGui::GetFrameHeightWithSpacing() + 10;

        ImGui::InputTextMultiline((id + "editor").c_str(), contentBuffer, sizeof(contentBuffer), contentSize, ImGuiInputTextFlags_AllowTabInput);

        if (ImGui::Button((Icons::save + " Save" + id + "save").c_str(), ImVec2(contentSize.x, 0))) {
            saveFile();
        }
    }

    void AssetEditorPanel::renderMaterialEditor() {
        if (!currentMaterial) return;

        static std::shared_ptr<FormPanel> materialForm;
        if (!materialForm) {
            materialForm = std::make_shared<FormPanel>();
            materialForm->onInitialize();
        }

        materialForm->resetForm();
        materialForm->setInspection(currentMaterial.get());
        materialForm->onSync();

        if (ImGui::Button((Icons::save + " Save Material" + id + "saveMat").c_str(), ImVec2(-1, 0))) {
            saveFile();
        }
    }

    void AssetEditorPanel::loadFile(const std::string& path) {
        if (!fs::exists(path) || fs::is_directory(path)) {
            return;
        }

        filePath = path;
        std::strncpy(pathInput, path.c_str(), sizeof(pathInput) - 1);
        
        auto ftime = fs::last_write_time(path);
        auto sys_tp = std::chrono::file_clock::to_utc(ftime);
        lastModified = std::format("{:%Y-%m-%d %H:%M:%S}", sys_tp);

        if (path.ends_with(".mmat")) {
            isMaterial = true;
            currentMaterial = std::make_shared<Material>();
            std::ifstream is(path);
            if (is.is_open()) {
                try {
                    nlohmann::json j;
                    is >> j;
                    currentMaterial->fromJson(j);
                } catch (...) {
                    // Handle error or just leave default
                }
            }
        } else {
            isMaterial = false;
            currentMaterial = nullptr;
            std::ifstream file(path);
            if (file.is_open()) {
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                std::strncpy(contentBuffer, content.c_str(), sizeof(contentBuffer) - 1);
                contentBuffer[sizeof(contentBuffer) - 1] = '\0';
            }
        }
    }

    void AssetEditorPanel::saveFile() {
        if (filePath.empty()) return;

        std::ofstream file(filePath);
        if (file.is_open()) {
            if (isMaterial && currentMaterial) {
                file << currentMaterial->toJson().dump(4);
            } else {
                file << contentBuffer;
            }
            file.close();
            
            auto ftime = fs::last_write_time(filePath);
            auto sys_tp = std::chrono::file_clock::to_utc(ftime);
            lastModified = std::format("{:%Y-%m-%d %H:%M:%S}", sys_tp);
        }
    }
}
