#ifndef VOXELIZERSERVICE_H
#define VOXELIZERSERVICE_H
#include <string>
#include <unordered_map>
#include <glm/mat4x4.hpp>

#include "../../common/AbstractRuntimeComponent.h"
#include "impl/OctreeNode.h"
#include "impl/SparseVoxelOctreeData.h"
#include "impl/Triangle.h"

namespace Metal {
    struct MeshComponent;
    class SparseVoxelOctreeBuilder;
    struct MeshData;

    class VoxelizationService final : public AbstractRuntimeComponent {
        void iterateTriangle(const MeshComponent *component, const Triangle &triangle,
                             std::unordered_map<std::string, SparseVoxelOctreeBuilder> &builders) const;

        void voxelize(const MeshComponent *component, const glm::mat4x4 &modelMatrix, const MeshData *mesh,
                      std::unordered_map<std::string, SparseVoxelOctreeBuilder> &builders) const;

        static void FillStorage(unsigned int targetDepth, unsigned int &bufferIndex,
                                unsigned int &materialBufferIndex,
                                SparseVoxelOctreeData &voxels, OctreeNode *node);

        static void PutData(unsigned int &bufferIndex, OctreeNode *node);

        void serialize(SparseVoxelOctreeBuilder &builder) const;

    public:
        explicit VoxelizationService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        void voxelizeScene() const;
    };
} // Metal

#endif //VOXELIZERSERVICE_H
