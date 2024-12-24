#ifndef FILESUTIL_H
#define FILESUTIL_H
#include <filesystem>
#include "../dto/file/FileEntry.h"

namespace fs = std::filesystem;

namespace Metal::FilesUtil {
    static void ReadFile(const char *pFileName, std::string &outFile) {
        if (std::ifstream f(pFileName); f.is_open()) {
            std::string line;
            while (getline(f, line)) {
                outFile.append(line);
                outFile.append("\n");
            }

            f.close();
        }
    }

    static void WriteBinaryFile(const char *pFilename, const void *pData, size_t size) {
        FILE *f = fopen(pFilename, "wb");
        if (!f) {
            throw std::runtime_error("Error opening file : " + std::string(pFilename));
        }

        if (const size_t bytes_written = fwrite(pData, 1, size, f); bytes_written != size) {
            throw std::runtime_error("Error writing to binary file: " + std::string(pFilename));
        }
        fclose(f);
    }

    static void WriteFile(const char *filename, const char *content) {
        std::ofstream file;
        file.open(filename, std::fstream::trunc);
        file << content;
        file.close();
    }

    static void MkDir(const std::string &pathToDir) {
        if (std::filesystem::exists(pathToDir)) {
            return;
        }
        std::filesystem::create_directory(pathToDir);
    }
} // Metal
#endif
