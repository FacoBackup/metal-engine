#ifndef LEVELOFDETAIL_H
#define LEVELOFDETAIL_H
#include <string>
#include <array>
#define LOD_COUNT 4

namespace Metal {
    struct LevelOfDetail {
        static LevelOfDetail LOD_0;
        static LevelOfDetail LOD_1;
        static LevelOfDetail LOD_2;
        static LevelOfDetail LOD_3;
        static std::array<LevelOfDetail, 4> LODs;

        const int level;
        const std::string suffix;

        static LevelOfDetail &OfNumber(unsigned int level);
    };
}
#endif //LEVELOFDETAIL_H
