#ifndef FILESSERVICE_H
#define FILESSERVICE_H
#include <iostream>
#include <filesystem>
#include <vector>
#include "FileEntry.h"
#include "../../../common/interface/engine-definitions.h"
#include <ctime>

namespace fs = std::filesystem;

namespace Metal::FilesUtil {
    static EntryType GetExtensionType(const std::string &extension) {
        const std::string ext = extension.substr(extension.find_last_of(FILE_METADATA) + 1);
        if (ext == FILE_MESH)
            return EntryType::MESH;
        if (ext == FILE_TEXTURE)
            return EntryType::TEXTURE;
        if (ext == FILE_MATERIAL)
            return EntryType::MATERIAL;
        return EntryType::NONE;
    }

    static std::vector<FileEntry> GetEntries(const std::string &directoryPath) {
        std::vector<FileEntry> metaFiles;

        try {
            for (const auto &entry: fs::directory_iterator(directoryPath)) {
                if (entry.is_regular_file()) {
                    std::string extension = entry.path().extension().string();
                    auto type = GetExtensionType(entry.path().extension().string());
                    if (type != EntryType::NONE && extension.find(FILE_METADATA) != std::string::npos) {
                        auto fileSizeInBytes = fs::file_size(entry.path());
                        std::string fileSize;

                        if (fileSizeInBytes >= 1'000'000'000) {
                            fileSize = std::to_string(fileSizeInBytes / 1'000'000'000.0) + " GB";
                        } else if (fileSizeInBytes >= 1'000'000) {
                            fileSize = std::to_string(fileSizeInBytes / 1'000'000.0) + " MB";
                        } else if (fileSizeInBytes >= 1'000) {
                            fileSize = std::to_string(fileSizeInBytes / 1'000.0) + " KB";
                        } else {
                            fileSize = std::to_string(fileSizeInBytes) + " Bytes";
                        }

                        std::time_t timeT = decltype(last_write_time(entry))::clock::now().time_since_epoch().count();
                        std::string lastChangeDate = std::asctime(std::localtime(&timeT));
                        lastChangeDate.pop_back(); // Remove trailing newline

                        metaFiles.emplace_back(
                            entry.path().filename().string(),
                            fs::absolute(entry.path()).string(),
                            lastChangeDate,
                            type,
                            fileSize
                        );
                    }
                } else {
                    metaFiles.emplace_back(entry.path().filename(), fs::absolute(entry.path()).string(),
                                           "",
                                           EntryType::DIRECTORY, "0");
                }
            }
        } catch (const std::exception &ex) {
            std::cerr << "Error: " << ex.what() << std::endl;
        }

        return metaFiles;
    }

    static void GetDirectoriesFromRoot(FileEntry &root) {
        try {
            if (fs::exists(root.absolutePath) && fs::is_directory(root.absolutePath)) {
                for (const auto &entry: fs::recursive_directory_iterator(root.absolutePath)) {
                    if (fs::is_directory(entry.status())) {
                        root.childDirectories.emplace_back(entry.path().filename(), fs::absolute(entry.path()).string(),
                                                           "",
                                                           EntryType::DIRECTORY, "0");
                    }
                }
            } else {
                std::cerr << "The provided path is not a valid directory.\n";
            }
        } catch (const std::filesystem::filesystem_error &e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        }
    }

    inline void WriteBinaryFile(const char *pFilename, const void *pData, size_t size) {
        FILE *f = fopen(pFilename, "wb");
        if (!f) {
            throw std::runtime_error("Error opening file : " + std::string(pFilename));
        }

        if (const size_t bytes_written = fwrite(pData, 1, size, f); bytes_written != size) {
            throw std::runtime_error("Error writing to binary file: " + std::string(pFilename));
        }
        fclose(f);
    }

    inline void ReadFile(const char *pFileName, std::string &outFile) {
        if (std::ifstream f(pFileName); f.is_open()) {
            std::string line;
            while (getline(f, line)) {
                outFile.append(line);
                outFile.append("\n");
            }

            f.close();
        }
    }

    static void WriteFile(const char *filename, const char *content) {
        std::ofstream file;
        file.open(filename, std::fstream::trunc);
        file << content;
        file.close();
    }

    static void MkDir(const std::string &pathToDir) {
        std::filesystem::create_directory(pathToDir);
    }
} // Metal

#endif //FILESSERVICE_H
