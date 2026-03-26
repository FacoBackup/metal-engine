#ifndef FILESSERVICE_H
#define FILESSERVICE_H
#include <string>
#include <unordered_map>
#include <memory>

#include "common/IInit.h"
#include "common/IService.h"

namespace Metal {
    struct DirectoryService;
    struct FSEntry;

    class NotificationService;

    class FilesService final : public IService, public IInit {
        std::shared_ptr<FSEntry> root = nullptr;
        DirectoryService *directoryService = nullptr;
        NotificationService *notificationService = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"DirectoryService", &directoryService},
                {"NotificationService", &notificationService}
            };
        }

        std::shared_ptr<FSEntry> getRoot() const {
            return root;
        }

        void onInitialize() override;

        std::shared_ptr<FSEntry> getResource(const std::string &id) const;

        void deleteFiles(const std::unordered_map<std::string, std::shared_ptr<FSEntry> > &files_context);

        void Move(const std::shared_ptr<FSEntry> &toMove, const std::shared_ptr<FSEntry> &targetDir) const;

        void CreateDirectory(const std::shared_ptr<FSEntry> &currentDirectory);

        void CreateFile(const std::shared_ptr<FSEntry> &currentDirectory, const std::string &name,
                        const std::string &extension);

        void GetEntries(const std::shared_ptr<FSEntry> &root);

        std::shared_ptr<FSEntry> GetEntry(const std::string &path);

        std::vector<std::string> listFilesWithExtension(const std::string &extension) const;

        /**
         * Writes content to a file in the root directory.
         * @param name Filename.
         * @param content Content to write.
         * @return The absolute path of the created file, or empty string on error.
         */
        std::string writeRootFile(const std::string &name, const std::string &content);
    };
} // Metal

#endif //FILESSERVICE_H
