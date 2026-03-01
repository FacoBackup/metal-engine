#ifndef MESHMETADATA_H
#define MESHMETADATA_H

namespace Metal {
    struct MeshMetadata {
        unsigned int renderIndex;
        unsigned int materialIndex;
        unsigned int padding0;
        unsigned int padding1;
    };
}

#endif //MESHMETADATA_H
