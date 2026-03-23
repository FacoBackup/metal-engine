#ifndef UTIL_H
#define UTIL_H
#include <fstream>
#include <random>
#include <sstream>
#include <string>

#define MAX_INT std::numeric_limits<int>::max()
#define MAX_FLOAT std::numeric_limits<float>::max()

namespace Metal::Util {
    static float TO_RADIANS = (3.1415926535f / 180);
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    std::string getTypeName(std::string name, bool toLower = true);

    std::string uuidV4();
}
#endif //UTIL_H
