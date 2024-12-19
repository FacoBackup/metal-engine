#include "LevelOfDetail.h"

#include "../../common/util/files/EntryType.h"
#include "../engine-definitions.h"

namespace Metal {
    LevelOfDetail LevelOfDetail::LOD_0{1, "LOD-0"};
    LevelOfDetail LevelOfDetail::LOD_1{2, "LOD-1"};
    LevelOfDetail LevelOfDetail::LOD_2{4, "LOD-2"};
    LevelOfDetail LevelOfDetail::LOD_3{8, "LOD-3"};
}
