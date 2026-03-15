#ifndef LIGHTDATA_H
#define LIGHTDATA_H

namespace Metal {
    struct LightData final {
        alignas(4) unsigned int triangleIndex;
        alignas(4) unsigned int meshIndex;
    };
}

#endif //LIGHTDATA_H
