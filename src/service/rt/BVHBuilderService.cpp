#include "BVHBuilderService.h"

#include "../../context/ApplicationContext.h"
#include "../../enum/LevelOfDetail.h"
#include "../../service/mesh/MeshData.h"
#include "data/MeshBVH.h"
#include "data/RTTriangle.h"

namespace Metal {
    int BVHBuilderService::buildBLAS(MeshData *mesh, BVH &bvh) {
        auto instance = MeshBVH(mesh);
        for (auto &data: instance.allTris) {
            bvh.triangles.push_back(data);
        }
        unsigned int offset = bvh.blas.size();
        for (auto &data: instance.allNodes) {
            bvh.blas.push_back(data);
        }
        return offset;
    }

    BVH BVHBuilderService::buildBVH() const {
        BVH bvh{};
        for (auto &t: context.worldGridRepository.getTiles()) {
            for (auto entity: t.second.entities) {
                if (context.worldRepository.meshes.contains(entity) && !context.worldRepository.hiddenEntities.
                    contains(entity)) {
                    auto &meshComponent = context.worldRepository.meshes.at(entity);
                    auto *mesh = context.meshService.stream(meshComponent.meshId, LevelOfDetail::LOD_0);
                    if (mesh != nullptr) {
                        auto &tlas = bvh.tlas.emplace_back();
                        tlas.id = entity;
                        tlas.bottomLevelASOffset = buildBLAS(mesh, bvh);
                        delete mesh;
                    }
                }
            }
        }
        return bvh;
    }
} // Metal
