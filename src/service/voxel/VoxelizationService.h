#ifndef VOXELIZERSERVICE_H
#define VOXELIZERSERVICE_H
#include <string>
#include <thread>
#include <unordered_map>
#include <glm/mat4x4.hpp>

#include "../../common/AbstractRuntimeComponent.h"
#include "impl/OctreeNode.h"
#include "impl/SparseVoxelOctreeData.h"
#include "impl/Triangle.h"

namespace Metal {
    struct WorldTile;
    struct VoxelizationRequest;
    struct TextureData;
    struct MeshComponent;
    class SparseVoxelOctreeBuilder;
    struct MeshData;

    class VoxelizationService final : public AbstractRuntimeComponent {
        std::unordered_map<std::string, TextureData *> textures{};
        std::unordered_map<std::string, SparseVoxelOctreeBuilder> builders{};
        std::string localVoxelizationRequestId;
        bool isVoxelizationDone = false;
        bool isVoxelizationCancelled = false;
        bool isExecutingThread = false;
        std::thread thread;

        void iterateTriangle(const MeshComponent *component, const Triangle &triangle);

        void voxelize(const MeshComponent *component, const glm::mat4x4 &modelMatrix, const MeshData *mesh);

        static bool isTriangleFlatInAxis(const Triangle &triangle);

        static void FillStorage(SparseVoxelOctreeBuilder &builder, unsigned int &bufferIndex,
                                unsigned int &materialBufferIndex,
                                SparseVoxelOctreeData &voxels, OctreeNode *node);

        static void PutData(unsigned int &bufferIndex, OctreeNode *node);

        void serialize(SparseVoxelOctreeBuilder &builder) const;

        void voxelize();

        void voxelizeGroup(const std::vector<VoxelizationRequest> &request);

        void collectRequests(WorldTile &t, std::vector<std::vector<VoxelizationRequest>> &requests) const;

    public:
        explicit VoxelizationService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        bool isExecutingVoxelization() const {
            return isExecutingThread;
        }

        void onSync() override;

        void cancelRequest();
    };
} // Metal

#endif //VOXELIZERSERVICE_H
