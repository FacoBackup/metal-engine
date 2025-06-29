#ifndef GRIDPUSHCONSTANT_H
#define GRIDPUSHCONSTANT_H

namespace Metal {
    struct GridPC final {
        alignas(4) float scale;
        alignas(4) int threshold;
        alignas(4) float thickness;
    };
}

#endif //GRIDPUSHCONSTANT_H
