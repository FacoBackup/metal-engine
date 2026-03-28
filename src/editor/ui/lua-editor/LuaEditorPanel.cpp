#include "LuaEditorPanel.h"
#include "imgui.h"
#include "common/Icons.h"
#include "editor/ui/UIUtil.h"
#include "common/FileDialogUtil.h"
#include <fstream>
#include <filesystem>
#include <chrono>
#include <format>
#include <cstring>
#include <nfd.hpp>

namespace fs = std::filesystem;

namespace Metal {
    LuaEditorPanel::LuaEditorPanel() {
        std::memset(pathInput, 0, sizeof(pathInput));
        std::memset(contentBuffer, 0, sizeof(contentBuffer));
    }

    void LuaEditorPanel::onSync() {
        ImGui::Text("%s Lua Script Editor", Icons::code.c_str());
        ImGui::Separator();

        ImGui::InputText("File Path", pathInput, sizeof(pathInput), ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        if (ImGui::Button((Icons::folder_open + "##load").c_str())) {
            std::vector<nfdu8filteritem_t> filters = {{"Lua Script", "lua"}};
            std::string selected = FileDialogUtil::PickFile(filters);
            if (!selected.empty()) {
                loadFile(selected);
            }
        }
        UIUtil::RenderTooltip("Load File");

        if (!lastModified.empty()) {
            ImGui::Text("Last Modified: %s", lastModified.c_str());
        } else {
            ImGui::Text("No file loaded");
        }

        ImGui::Separator();

        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        contentSize.y -= ImGui::GetFrameHeightWithSpacing() + 10; // Leave space for Save button

        ImGui::InputTextMultiline((id + "editor").c_str(), contentBuffer, sizeof(contentBuffer), contentSize, ImGuiInputTextFlags_AllowTabInput);

        if (ImGui::Button((Icons::save + " Save" + id + "save").c_str(), ImVec2(contentSize.x, 0))) {
            saveFile();
        }
    }

    void LuaEditorPanel::loadFile(const std::string& path) {
        if (!fs::exists(path) || fs::is_directory(path)) {
            return;
        }

        std::ifstream file(path);
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            filePath = path;
            std::strncpy(contentBuffer, content.c_str(), sizeof(contentBuffer) - 1);
            contentBuffer[sizeof(contentBuffer) - 1] = '\0';
            
            auto ftime = fs::last_write_time(path);
            auto sys_tp = std::chrono::file_clock::to_utc(ftime);
            lastModified = std::format("{:%Y-%m-%d %H:%M:%S}", sys_tp);
            
            std::strncpy(pathInput, path.c_str(), sizeof(pathInput) - 1);
        }
    }

    void LuaEditorPanel::saveFile() {
        if (filePath.empty()) return;

        std::ofstream file(filePath);
        if (file.is_open()) {
            file << contentBuffer;
            file.close();
            
            auto ftime = fs::last_write_time(filePath);
            auto sys_tp = std::chrono::file_clock::to_utc(ftime);
            lastModified = std::format("{:%Y-%m-%d %H:%M:%S}", sys_tp);
        }
    }
}
