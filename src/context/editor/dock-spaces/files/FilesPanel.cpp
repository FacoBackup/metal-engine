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
        if (UIUtil::ButtonSimple(Icons::inventory_2 + id + "CreateFolder", UIUtil::ONLY_ICON_BUTTON_SIZE,
                                 UIUtil::ONLY_ICON_BUTTON_SIZE)) {
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

    std::vector<std::string> FilesPanel::getDirectories(const std::string &path) {
        if (const auto it = directoryCache.find(path); it != directoryCache.end()) {
            return it->second;
        }

        std::vector<std::string> dirs;
        try {
            for (const auto &entry: fs::directory_iterator(path)) {
                if (entry.is_directory()) {
                    dirs.push_back(entry.path().filename().string());
                }
            }
        } catch (const fs::filesystem_error &e) {
            std::cerr << e.what() << std::endl;
        }

        directoryCache[path] = dirs;
        return dirs;
    }

    void FilesPanel::RenderDirectoryTree(const std::string &currentDir) {
        for (const auto &fullPath: getDirectories(currentDir)) {
            std::string dirName = std::filesystem::path(fullPath).filename().string();

            ImGuiTreeNodeFlags flags = (currentPath == fullPath) ? ImGuiTreeNodeFlags_Selected : 0;
            bool nodeOpen = ImGui::TreeNodeEx((Icons::inventory_2 + dirName).c_str(), flags);

            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
                history.emplace(currentPath);
                currentPath = fullPath;
                forwardHistory = {};
                fetched = false;
            }

            if (nodeOpen) {
                RenderDirectoryTree(fullPath);
                ImGui::TreePop();
            }
        }
    }

    void FilesPanel::onSync() {
        // Navigation Header
        renderNavigationHeader();

        if (directories.empty() || files.empty() && !fetched) {
            refreshDirectoryContents();
            fetched = true;
        }

        ImGui::BeginChild((id + "Directories").c_str(), ImVec2(200, 0), ImGuiWindowFlags_NoBackground);
        RenderDirectoryTree(rootPath);
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
} // Metal
