#ifndef PRIMITIVEDATA_H
#define PRIMITIVEDATA_H

#include <cstdint>

namespace Metal {
    struct PrimitiveData {
        unsigned int materialIndex;
        uint64_t vertexBufferAddress;
        uint64_t indexBufferAddress;
    };
}

#endif //PRIMITIVEDATA_H
