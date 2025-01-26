#ifndef UTIL_H
#define UTIL_H
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <__random/random_device.h>

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
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        };
        return ss.str();
    }

    template<typename T>
    int indexOf(const std::vector<T> &v, T val) {
        auto it = std::find(v.begin(), v.end(), val); // Find the iterator
        if (it != v.end()) {
            return std::distance(v.begin(), it); // Calculate the index
        }
        return -1; // Return -1 if not found
    }
}
#endif //UTIL_H
