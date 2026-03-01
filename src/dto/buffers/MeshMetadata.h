#ifndef MESHMETADATA_H
#define MESHMETADATA_H

#include <cstdint>

namespace Metal {
    struct MeshMetadata {
        unsigned int renderIndex;
        unsigned int materialIndex;
        uint64_t vertexBufferAddress;
        uint64_t indexBufferAddress;
    };
}

#endif //MESHMETADATA_H
