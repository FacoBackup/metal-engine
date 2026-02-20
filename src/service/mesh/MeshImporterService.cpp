#include "MeshImporterService.h"
#include "MeshData.h"
#include "VertexData.h"
#include "../../dto/file/EntryMetadata.h"
#include "../../enum/engine-definitions.h"
#include "../../enum/LevelOfDetail.h"
#include "../../util/FilesUtil.h"
#include "../../context/ApplicationContext.h"
#include <meshoptimizer.h>

#include <cereal/archives/binary.hpp>

namespace Metal {
    std::string MeshImporterService::persistMesh(const std::string &targetDir, const MeshData &mesh) const {
        auto metadata = EntryMetadata{};
        metadata.type = EntryType::MESH;
        metadata.name = mesh.name;
        if (metadata.name.find_last_of('.') != std::string::npos) {
            metadata.name = metadata.name.substr(0, metadata.name.find_last_of('.'));
        }
        DUMP_TEMPLATE(targetDir + '/' + metadata.getId() + FILE_METADATA, metadata)
        DUMP_TEMPLATE(CTX.getAssetDirectory() + FORMAT_FILE_MESH(metadata.getId(), LevelOfDetail::LOD_0), mesh)
        simplifyMesh(metadata.getId(), mesh, LevelOfDetail::LOD_1);
        simplifyMesh(metadata.getId(), mesh, LevelOfDetail::LOD_2);
        simplifyMesh(metadata.getId(), mesh, LevelOfDetail::LOD_3);
        return metadata.getId();
    }

    void MeshImporterService::persistAllMeshes(const std::string &targetDir, const aiScene *scene,
                                                std::unordered_map<unsigned int, std::string> &meshMap,
                                                std::unordered_map<std::string, unsigned int> &meshMaterialMap,
                                                const std::stop_token &stopToken) const {
        LOG_INFO("Processing meshes for scene...");
        for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
            if (stopToken.stop_requested()) return;
            aiMesh *assimpMesh = scene->mMeshes[i];
            MeshData meshData{assimpMesh->mName.data, {}, {}, {}};
            glm::vec3 center(0.0f);
            for (unsigned int j = 0; j < assimpMesh->mNumVertices; ++j) {
                VertexData vertexData{};
                vertexData.vertex = glm::vec3(
                    assimpMesh->mVertices[j].x,
                    assimpMesh->mVertices[j].y,
                    assimpMesh->mVertices[j].z
                );
                center += vertexData.vertex;
                if (assimpMesh->HasNormals()) {
                    vertexData.normal = glm::vec3(
                        assimpMesh->mNormals[j].x,
                        assimpMesh->mNormals[j].y,
                        assimpMesh->mNormals[j].z
                    );
                } else {
                    vertexData.normal = glm::vec3(0.0f);
                }
                if (assimpMesh->HasTextureCoords(0)) {
                    vertexData.uv = glm::vec2(
                        assimpMesh->mTextureCoords[0][j].x,
                        assimpMesh->mTextureCoords[0][j].y
                    );
                } else {
                    vertexData.uv = glm::vec2(0.0f);
                }

                meshData.data.push_back(vertexData);
            }
            if (assimpMesh->mNumVertices > 0) {
                meshData.gizmoCenter = center / static_cast<float>(assimpMesh->mNumVertices);
            }
            for (unsigned int j = 0; j < assimpMesh->mNumFaces; ++j) {
                aiFace face = assimpMesh->mFaces[j];
                for (unsigned int k = 0; k < face.mNumIndices; ++k) {
                    meshData.indices.push_back(face.mIndices[k]);
                }
            }
            std::string id = persistMesh(targetDir, meshData);
            meshMap.insert({i, id});
            meshMaterialMap.insert({id, assimpMesh->mMaterialIndex});
            LOG_INFO("Persisted mesh: " + meshData.name + " (" + id + ")");
        }
    }

    void MeshImporterService::simplifyMesh(const std::string &fileId, const MeshData &mesh,
                                            const LevelOfDetail &levelOfDetail) const {
        size_t vertexCount = mesh.data.size();
        size_t indexCount = mesh.indices.size();
        size_t targetIndexCount = indexCount / levelOfDetail.level;

        std::vector<unsigned int> simplifiedIndices(indexCount);
        float target_error = 1e-2f;
        float lod_error = 0.f;

        size_t simplifiedIndexCount = meshopt_simplify(
            simplifiedIndices.data(),
            mesh.indices.data(),
            indexCount,
            &mesh.data[0].vertex.x,
            vertexCount,
            sizeof(VertexData),
            targetIndexCount,
            target_error,
            0,
            &lod_error
        );

        simplifiedIndices.resize(simplifiedIndexCount);

        MeshData simplifiedMesh;
        simplifiedMesh.data = mesh.data;
        simplifiedMesh.indices = std::move(simplifiedIndices);

        DUMP_TEMPLATE(CTX.getAssetDirectory() + FORMAT_FILE_MESH(fileId, levelOfDetail), simplifiedMesh)
    }
}
