#ifndef MESHMETADATA_H
#define MESHMETADATA_H

#include <cstdint>
#include <glm/vec3.hpp>

namespace Metal {
    struct MeshMetadata {
        unsigned int renderIndex;
        uint64_t vertexBufferAddress;
        uint64_t indexBufferAddress;

        alignas(16) glm::vec3 albedo{};
        alignas(4) float roughness{};
        alignas(4) float metallic{};
        alignas(4) float transmission{};
        alignas(4) float thickness{};
        alignas(4) float ior{1.45f};
        alignas(4) unsigned int isEmissive{};

        alignas(4) unsigned int useAlbedoTexture;
        alignas(4) unsigned int useNormalTexture;
        alignas(4) unsigned int useRoughnessTexture;
        alignas(4) unsigned int useMetallicTexture;

        alignas(4) unsigned int albedoTextureId = 0;
        alignas(4) unsigned int normalTextureId = 0;
        alignas(4) unsigned int roughnessTextureId = 0;
        alignas(4) unsigned int metallicTextureId = 0;
    };
}

#endif //MESHMETADATA_H
