#include "BVHBuilderService.h"

#include "../../MetalContextProvider.h"
#include "../../common/enum/LevelOfDetail.h"
#include "../data/MeshData.h"
#include "../data/MeshBVH.h"
#include "../data/TriangleBuffer.h"

namespace Metal {
    BVH BVHBuilderService::buildBVH() const {
        BVH bvh{};
        for (auto &t: SINGLETONS.worldRepository.entities) {
            EntityID entity = t.first;
            if (SINGLETONS.worldRepository.meshes.contains(entity) && !SINGLETONS.worldRepository.hiddenEntities.
                contains(entity)) {
                auto &meshComponent = SINGLETONS.worldRepository.meshes.at(entity);
                auto *mesh = SINGLETONS.meshService.stream(meshComponent.meshId, LevelOfDetail::LOD_0);
                if (mesh != nullptr) {
                    auto instance = MeshBVH(mesh);

                    auto &tlas = bvh.tlas.emplace_back();
                    tlas.id = entity;
                    tlas.triangleOffset = bvh.triangles.size();
                    tlas.nodeOffset = bvh.blas.size();

                    for (auto &data: instance.allTris) {
                        bvh.triangles.push_back(data);
                    }
                    for (auto &data: instance.allNodes) {
                        bvh.blas.push_back(data);
                    }


                    unsigned int stackIndex = 0;
                    std::array<int, 32> nodes{};
                    nodes[stackIndex++] = 0;
                    while (stackIndex > 0) {
                        auto index = nodes.at(--stackIndex);
                        BLASBuffer *currentNode = &instance.allNodes[index];

                        if (currentNode->triangleCount <= 0) {
                            nodes[stackIndex++] = currentNode->startIndex;
                            nodes[stackIndex++] = currentNode->startIndex + 1;
                        }
                    }

                    delete mesh;
                }
            }
        }
        return bvh;
    }
} // Metal
