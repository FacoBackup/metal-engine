#include "FilesPanel.h"


#include "FilesHeader.h"
#include "../../../../common/interface/Icons.h"
#include "../../../../util/FilesUtil.h"
#include "../../../../context/ApplicationContext.h"
#include "../../../../service/mesh/SceneData.h"
#include "../../../../util/UIUtil.h"

namespace Metal {
    void FilesPanel::onInitialize() {
        rootPath = context->getRootDirectory();
        currentPath = rootPath;
    }

    void FilesPanel::onSync() {
        // Navigation Header
        renderNavigationHeader();

        if (directories.empty() || files.empty()) {
            refreshDirectoryContents();
        }

        ImGui::BeginChild((id + "Directories").c_str(), ImVec2(200, 0), ImGuiWindowFlags_NoBackground);
        for (const auto &dir: directories) {
            if (ImGui::Selectable( (Icons::inventory_2 + dir).c_str(), currentPath == (rootPath + "/" + dir))) {
                history.emplace(currentPath);
                currentPath = rootPath + "/" + dir;
                forwardHistory = {};
                refreshDirectoryContents();
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild((id + "Files").c_str(), ImVec2(400, 0), ImGuiWindowFlags_NoBackground);
        std::vector<FileItem> filteredFiles = files;
        if (!searchBuffer[0] == '\0') {
            filteredFiles.erase(std::remove_if(filteredFiles.begin(), filteredFiles.end(),
                                               [&](const FileItem &file) {
                                                   return file.name.find(searchBuffer) != std::string::npos;
                                               }),
                                filteredFiles.end());
        }

        for (const auto &file: filteredFiles) {
            if (ImGui::Selectable(file.name.c_str(), selectedFile == file.name)) {
                selectedFile = file.name;
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild((id + "Preview").c_str(), ImVec2(0, 0), ImGuiWindowFlags_NoBackground);
        if (!selectedFile.empty()) {
            ImGui::Text("No supported files selected");
        }
        ImGui::EndChild();
    }


    void FilesPanel::refreshDirectoryContents() {
        directories.clear();
        files.clear();
        try {
            for (const auto &entry: fs::directory_iterator(currentPath)) {
                if (entry.is_directory()) {
                    directories.push_back(entry.path().filename().string());
                } else {
                    files.push_back({entry.path().filename().string(), false});
                }
            }
        } catch (const fs::filesystem_error &e) {
            std::cout << "Error accessing directory: " << e.what() << std::endl;
        }
    }

    void FilesPanel::renderNavigationHeader() {
        ImGui::BeginGroup();

        if (UIUtil::ButtonSimple(Icons::arrow_left, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            if (!history.empty()) {
                forwardHistory.emplace(currentPath);
                currentPath = history.top();
                history.pop();
                refreshDirectoryContents();
            }
        }

        ImGui::SameLine();

        if (UIUtil::ButtonSimple(Icons::arrow_right, UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            if (!forwardHistory.empty()) {
                history.emplace(currentPath);
                currentPath = forwardHistory.top();
                forwardHistory.pop();
                refreshDirectoryContents();
            }
        }

        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::InputText((id + "Search").c_str(), searchBuffer, IM_ARRAYSIZE(searchBuffer));

        ImGui::SameLine();
        auto createPopUp = ("Create Folder Popup" + id + "CreateFolderPopup").c_str();
        if (UIUtil::ButtonSimple(Icons::inventory_2 + id + "CreateFolder", UIUtil::ONLY_ICON_BUTTON_SIZE, UIUtil::ONLY_ICON_BUTTON_SIZE)) {
            ImGui::OpenPopup(createPopUp);
        }

        if (ImGui::BeginPopup(createPopUp)) {
            static char folderName[256] = "";
            ImGui::InputText("Folder Name", folderName, IM_ARRAYSIZE(folderName));
            if (ImGui::Button("Create")) {
                try {
                    fs::create_directory(currentPath + "/" + folderName);
                    refreshDirectoryContents();
                    ImGui::CloseCurrentPopup();
                } catch (const fs::filesystem_error &e) {
                    ImGui::Text("Error creating folder: %s", e.what());
                }
            }
            ImGui::EndPopup();
        }

        ImGui::SameLine();
        ImGui::Text("Path: %s", currentPath.c_str());
        ImGui::EndGroup();
    }
} // Metal
