#include "MeshImporterService.h"
#include "MeshData.h"
#include "VertexData.h"
#include "../../dto/file/EntryMetadata.h"
#include "../../enum/engine-definitions.h"
#include "../../util/FilesUtil.h"
#include "../../context/ApplicationContext.h"
#include <cereal/archives/binary.hpp>
#include <meshoptimizer.h>
#include "../../util/serialization-definitions.h"

namespace Metal {
    std::string MeshImporterService::persistMesh(const std::string &targetDir, const MeshData &mesh) const {
        auto metadata = EntryMetadata{};
        metadata.type = EntryType::MESH;
        metadata.name = mesh.name;
        if (metadata.name.find_last_of('.') != std::string::npos) {
            metadata.name = metadata.name.substr(0, metadata.name.find_last_of('.'));
        }

        std::string lod0Path = CTX.getAssetDirectory() + FORMAT_FILE_MESH(metadata.getId());
        {
            std::ofstream output(lod0Path, std::ios::binary);
            cereal::BinaryOutputArchive archive(output);
            archive(mesh);
        }
        metadata.size += fs::file_size(lod0Path);

        DUMP_TEMPLATE(targetDir + '/' + metadata.getId() + FILE_METADATA, metadata)
        return metadata.getId();
    }

    void MeshImporterService::persistAllMeshes(const std::string &targetDir, const aiScene *scene,
                                                std::unordered_map<unsigned int, MeshId> &meshMap,
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
            meshMap.insert({i, {id, meshData.gizmoCenter}});
            LOG_INFO("Persisted mesh: " + meshData.name + " (" + id + ")");
        }
    }
}
