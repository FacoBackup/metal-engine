#include "BVHBuilderService.h"

#include "../../context/ApplicationContext.h"
#include "../../enum/LevelOfDetail.h"
#include "../../service/mesh/MeshData.h"
#include "data/RTTriangle.h"

namespace Metal {
    void BVHBuilderService::buildBLAS(MeshData *mesh, BVH &bvh) {
        auto &indices = mesh->indices;
        auto &data = mesh->data;
        auto &blas = bvh.blas.emplace_back();
        blas.maxOffset.w = bvh.triangles.size();
        blas.minCount.w = indices.size() / 3;

        for (int i = 0; i < indices.size(); i += 3) {
            const auto index0 = indices[i];
            const auto index1 = indices[i + 1];
            const auto index2 = indices[i + 2];

            glm::vec3 v0 = data[index0].vertex;
            glm::vec3 v1 = data[index1].vertex;
            glm::vec3 v2 = data[index2].vertex;

            bvh.triangles.emplace_back(RTTriangle{
                v0, v1, v2,
                data[index0].normal,
                data[index1].normal,
                data[index2].normal,
                data[index0].uv,
                data[index1].uv,
                data[index2].uv
            });

            blas.minCount.x = std::min({blas.minCount.x, v0.x, v1.x, v2.x});
            blas.minCount.y = std::min({blas.minCount.y, v0.y, v1.y, v2.y});
            blas.minCount.z = std::min({blas.minCount.z, v0.z, v1.z, v2.z});

            blas.maxOffset.x = std::max({blas.maxOffset.x, v0.x, v1.x, v2.x});
            blas.maxOffset.y = std::max({blas.maxOffset.y, v0.y, v1.y, v2.y});
            blas.maxOffset.z = std::max({blas.maxOffset.z, v0.z, v1.z, v2.z});
        }
    }

    void BVHBuilderService::buildTLAS(EntityID id, MeshData *mesh, BVH &bvh) {
        auto &tlas = bvh.tlas.emplace_back();
        tlas.id = id;
        tlas.bottomLevelASOffset = bvh.blas.size();
        buildBLAS(mesh, bvh);
    }

    BVH BVHBuilderService::buildBVH() {
        bvhVersion++;
        BVH bvh{};
        for (auto &t: context.worldGridRepository.getTiles()) {
            for (auto entity: t.second.entities) {
                if (context.worldRepository.meshes.contains(entity) && !context.worldRepository.hiddenEntities.
                    contains(entity)) {
                    auto &meshComponent = context.worldRepository.meshes.at(entity);
                    auto *mesh = context.meshService.stream(meshComponent.meshId, LevelOfDetail::LOD_0);
                    if (mesh != nullptr) {
                        meshComponent.bvhVersion = bvhVersion;
                        buildTLAS(entity, mesh, bvh);
                        delete mesh;
                    }
                }
            }
        }
        return bvh;
    }
} // Metal
