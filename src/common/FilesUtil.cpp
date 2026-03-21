#include "FilesUtil.h"

namespace Metal::FilesUtil {
    void ReadFile(const char *pFileName, std::string &outFile) {
        if (std::ifstream f(pFileName); f.is_open()) {
            std::string line;
            while (getline(f, line)) {
                outFile.append(line);
                outFile.append("\n");
            }

            f.close();
        }
    }

    void WriteBinaryFile(const char *pFilename, const void *pData, size_t size) {
        FILE *f = fopen(pFilename, "wb");
        if (!f) {
            throw std::runtime_error("Error opening file : " + std::string(pFilename));
        }

        if (const size_t bytes_written = fwrite(pData, 1, size, f); bytes_written != size) {
            throw std::runtime_error("Error writing to binary file: " + std::string(pFilename));
        }
        fclose(f);
    }

    void ReadBinaryFile(const char *pFilename, std::vector<unsigned int> &outData) {
        std::ifstream file(pFilename, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            throw std::runtime_error("Error opening binary file : " + std::string(pFilename));
        }

        const std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        outData.resize(size / sizeof(unsigned int));
        if (!file.read(reinterpret_cast<char *>(outData.data()), size)) {
            throw std::runtime_error("Error reading binary file: " + std::string(pFilename));
        }
    }

    void WriteFile(const char *filename, const char *content) {
        std::ofstream file;
        file.open(filename, std::fstream::trunc);
        file << content;
        file.close();
    }

    void CreateDirectory(const std::string &pathToDir) {
        if (std::filesystem::exists(pathToDir)) {
            return;
        }
        std::filesystem::create_directory(pathToDir);
    }

    std::string FormatSize(size_t sizeInBytes) {
        if (sizeInBytes >= 1'000'000'000) {
            return std::format("{:.2f} GB", sizeInBytes / 1'000'000'000.0);
        }
        if (sizeInBytes >= 1'000'000) {
            return std::format("{:.2f} MB", sizeInBytes / 1'000'000.0);
        }
        if (sizeInBytes >= 1'000) {
            return std::format("{:.2f} KB", sizeInBytes / 1'000.0);
        }
        return std::to_string(sizeInBytes) + " Bytes";
    }
} // Metal
