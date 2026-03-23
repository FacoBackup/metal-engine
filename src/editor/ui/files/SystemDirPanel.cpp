#include "SystemDirPanel.h"
#include "../../service/FilesService.h"
#include "editor/service/ThemeService.h"
#include "common/Icons.h"
#include "editor/ui/UIUtil.h"
#include "editor/dto/FSEntry.h"
#include <shlobj.h>
#include <windows.h>
#include <filesystem>
#include <imgui.h>

namespace Metal {
    std::string GetSystemPath(int csidl) {
        char path[MAX_PATH];
        if (SHGetSpecialFolderPathA(NULL, path, csidl, FALSE)) {
            return std::string(path);
        }
        return "";
    }

    void SystemDirPanel::onInitialize() {
        systemDirs.push_back({"Home", GetSystemPath(CSIDL_PROFILE), Icons::home});
        systemDirs.push_back({"Desktop", GetSystemPath(CSIDL_DESKTOP), Icons::desktop_windows});
        systemDirs.push_back({"Documents", GetSystemPath(CSIDL_MYDOCUMENTS), Icons::description});
        systemDirs.push_back({"Downloads", GetSystemPath(CSIDL_PROFILE) + "\\Downloads", Icons::download});
        systemDirs.push_back({"Pictures", GetSystemPath(CSIDL_MYPICTURES), Icons::image});
        systemDirs.push_back({"Videos", GetSystemPath(CSIDL_MYVIDEO), Icons::movie});
        systemDirs.push_back({"Music", GetSystemPath(CSIDL_MYMUSIC), Icons::music_note});
    }

    void SystemDirPanel::onSync() {
        if (UIUtil::Accordion(id, Icons::computer + " System", isOpen, themeService->palette3)) {
            for (const auto& dir : systemDirs) {
                if (dir.path.empty()) continue;

                bool isSelected = (filesContext.currentDirectory && filesContext.currentDirectory->absolutePath == dir.path);
                if (ImGui::Selectable((dir.icon + " " + dir.name).c_str(), isSelected)) {
                    std::shared_ptr<FSEntry> entry = filesService->GetEntry(dir.path);
                    if (entry) {
                        filesContext.setCurrentDirectory(entry);
                        filesService->GetEntries(entry);
                        filesContext.selected.clear();
                    }
                }
            }
            UIUtil::EndAccordion();
        }
    }
}
