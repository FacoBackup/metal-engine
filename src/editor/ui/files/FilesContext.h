#ifndef FILESCONTEXT_H
#define FILESCONTEXT_H

#include <string>
#include <unordered_map>
#include <memory>
#include <utility>


namespace Metal {
    class FilesService;
    struct FSEntry;

    enum class FilesViewMode {
        LIST,
        CARD
    };

    struct FilesContext final {
        FilesService *filesService = nullptr;
        std::shared_ptr<FSEntry> currentDirectory = nullptr;
        std::unordered_map<std::string, std::shared_ptr<FSEntry> > selected{};
        std::unordered_map<std::string, std::shared_ptr<FSEntry> > toCut{};
        char searchQuery[256] = "";
        FilesViewMode viewMode = FilesViewMode::LIST;

        explicit FilesContext(FilesService *filesService, std::shared_ptr<FSEntry> currentDirectory)
            : filesService(filesService), currentDirectory(std::move(currentDirectory)) {
        }
    };
}
#endif //FILESCONTEXT_H
