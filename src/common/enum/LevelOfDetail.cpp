#include "LevelOfDetail.h"

namespace Metal {
    LevelOfDetail LevelOfDetail::LOD_0{1, "LOD-0"};
    LevelOfDetail LevelOfDetail::LOD_1{2, "LOD-1"};
    LevelOfDetail LevelOfDetail::LOD_2{4, "LOD-2"};
    LevelOfDetail LevelOfDetail::LOD_3{8, "LOD-3"};
    std::array<LevelOfDetail, 4> LevelOfDetail::LODs{LOD_0, LOD_1, LOD_2, LOD_3};

    LevelOfDetail &LevelOfDetail::OfNumber(unsigned int level) {
        if (level == 0) return LOD_0;
        if (level == 1) return LOD_1;
        if (level == 2) return LOD_2;
        if (level == 3) return LOD_3;
        return LOD_0;
    }
}
