#ifndef FILESUTIL_H
#define FILESUTIL_H
#include <filesystem>
#include <format>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include "editor/dto/FSEntry.h"

namespace fs = std::filesystem;

namespace Metal::FilesUtil {
    void ReadFile(const char *pFileName, std::string &outFile);

    void WriteBinaryFile(const char *pFilename, const void *pData, size_t size);

    void ReadBinaryFile(const char *pFilename, std::vector<unsigned int> &outData);

    void WriteFile(const char *filename, const char *content);

    void CreateDirectory(const std::string &pathToDir);

    std::string FormatSize(size_t sizeInBytes);
} // Metal
#endif
