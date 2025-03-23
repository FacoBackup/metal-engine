#include "BVHBuilderService.h"

#include "../../context/ApplicationContext.h"
#include "../../enum/LevelOfDetail.h"
#include "../../service/mesh/MeshData.h"
#include "data/MeshBVH.h"
#include "../../dto/buffers/RTTriangle.h"

namespace Metal {
    BVH BVHBuilderService::buildBVH() const {
        BVH bvh{};
        for (auto &t: context.worldRepository.entities) {
            EntityID entity = t.first;
            if (context.worldRepository.meshes.contains(entity) && !context.worldRepository.hiddenEntities.
                contains(entity)) {
                auto &meshComponent = context.worldRepository.meshes.at(entity);
                auto *mesh = context.meshService.stream(meshComponent.meshId, LevelOfDetail::LOD_0);
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
                        BLAS *currentNode = &instance.allNodes[index];

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
