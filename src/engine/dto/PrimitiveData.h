#ifndef PRIMITIVEDATA_H
#define PRIMITIVEDATA_H

#include <cstdint>

namespace Metal {
    struct PrimitiveData {
        alignas(4) unsigned int materialIndex;
        alignas(4) unsigned int renderIndex;
        alignas(8) uint64_t vertexBufferAddress;
        alignas(8) uint64_t indexBufferAddress;
    };
}

#endif //PRIMITIVEDATA_H
