#ifndef UTIL_H
#define UTIL_H
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <__random/random_device.h>
#include <limits>

#define MAX_INT std::numeric_limits<int>::max()
#define MAX_FLOAT std::numeric_limits<float>::max()

namespace Metal::Util {
    static float TO_RADIANS = (3.1415926535f / 180);
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    inline std::string uuidV4() {
        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        };
        return ss.str();
    }

    template<typename T>
    int indexOf(const std::vector<T> &v, T val) {
        return std::find(v.begin(), v.end(), val) != v.end();
    }


    inline void WriteBinaryFile(const char *pFilename, const void *pData, int size) {
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
        } else {
            throw std::runtime_error("Error opening file : " + std::string(pFileName));
        }
    }
}
#endif //UTIL_H
