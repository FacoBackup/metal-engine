#ifndef VOXELIZERSERVICE_H
#define VOXELIZERSERVICE_H
#include <string>
#include <thread>
#include <unordered_map>
#include <glm/mat4x4.hpp>

#include "../../../common/AbstractRuntimeComponent.h"
#include "OctreeNode.h"
#include "SparseVoxelOctreeData.h"
#include "RTTriangle.h"

namespace Metal {
    struct WorldTile;
    struct VoxelizationRequest;
    struct TextureData;
    struct MeshComponent;
    class SparseVoxelOctreeBuilder;
    struct MeshData;

    class VoxelizerService final : public AbstractRuntimeComponent {
        void iterateTriangle(const MeshComponent *component, const RTTriangle &triangle);

        static bool isTriangleFlatInAxis(const RTTriangle &triangle);

    public:
        explicit VoxelizerService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        void voxelize(const MeshComponent *component, const glm::mat4x4 &modelMatrix, const MeshData *mesh);
    };
} // Metal

#endif //VOXELIZERSERVICE_H
