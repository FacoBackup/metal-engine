#ifndef GRIDPUSHCONSTANT_H
#define GRIDPUSHCONSTANT_H

namespace Metal {
    struct GridPushConstant final {
        alignas(4) bool overlayObjects;
        alignas(4) float scale;
        alignas(4) int threshold;
        alignas(4) float thickness;
    };
}

#endif //GRIDPUSHCONSTANT_H