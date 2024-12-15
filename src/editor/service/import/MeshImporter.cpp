#include "MeshImporter.h"


#include "../../../common/util/files/FileEntry.h"
#include "../../../context/runtime/MeshData.h"
#include "../../../context/runtime/VertexData.h"
#include <iostream>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "../../../common/util/files/FilesUtil.h"
#include "../../../common/util/files/FileMetadata.h"
#include "../../../engine/engine-definitions.h"
#include "../../../engine/enum/LevelOfDetail.h"
#include <meshoptimizer.h>

namespace Metal {
    std::vector<MeshData> MeshImporter::ImportMeshes(const std::string &pathToFile) {
        Assimp::Importer importer;

        const aiScene *scene = importer.ReadFile(
            pathToFile,
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GlobalScale | aiProcess_FindInstances |
            aiProcess_PreTransformVertices | aiProcess_GenSmoothNormals);

        if (!scene || !scene->HasMeshes()) {
            std::cerr << "Error loading scene: " << importer.GetErrorString() << std::endl;
            return {};
        }

        std::vector<MeshData> meshList;

        for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
            aiMesh *assimpMesh = scene->mMeshes[i];
            MeshData meshData{assimpMesh->mName.data, {}, {}};
            // Extract vertex data
            for (unsigned int j = 0; j < assimpMesh->mNumVertices; ++j) {
                VertexData vertexData{};

                // Vertex positions
                vertexData.vertex = glm::vec3(
                    assimpMesh->mVertices[j].x,
                    -assimpMesh->mVertices[j].y,
                    assimpMesh->mVertices[j].z
                );

                // Normals
                if (assimpMesh->HasNormals()) {
                    vertexData.normal = glm::vec3(
                        assimpMesh->mNormals[j].x,
                        assimpMesh->mNormals[j].y,
                        assimpMesh->mNormals[j].z
                    );
                } else {
                    vertexData.normal = glm::vec3(0.0f);
                }

                // Texture coordinates
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

            // Extract indices
            for (unsigned int j = 0; j < assimpMesh->mNumFaces; ++j) {
                aiFace face = assimpMesh->mFaces[j];
                for (unsigned int k = 0; k < face.mNumIndices; ++k) {
                    meshData.indices.push_back(face.mIndices[k]);
                }
            }

            meshList.push_back(meshData);
        }

        return meshList;
    }

    void MeshImporter::ImportMesh(const std::string &targetDir, const std::string &pathToFile) {
        auto metadata = FileMetadata{};
        metadata.type = EntryType::MESH;
        metadata.name = std::filesystem::path(pathToFile).filename().string();
        metadata.name = metadata.name.substr(0, metadata.name.find_last_of('.'));

        auto imported = ImportMeshes(pathToFile);
        for (MeshData &mesh: imported) {
            const std::string newPath = targetDir + "/" +
                                        LevelOfDetail::GetFormattedName(metadata.getId(), LevelOfDetail::LOD_0,
                                                                        EntryType::MESH);
            serializeMesh(mesh, newPath);
            metadata.associatedFiles.push_back(newPath);
            SimplifyMesh(metadata.associatedFiles, metadata.getId(), targetDir, mesh, LevelOfDetail::LOD_1);
            SimplifyMesh(metadata.associatedFiles, metadata.getId(), targetDir, mesh, LevelOfDetail::LOD_2);
            SimplifyMesh(metadata.associatedFiles, metadata.getId(), targetDir, mesh, LevelOfDetail::LOD_3);
        }
        FilesUtil::WriteFile((targetDir + '/' + metadata.getId() + FILE_METADATA).c_str(),
                             metadata.serialize().c_str());
    }

    void MeshImporter::serializeMesh(const MeshData &simplifiedMesh, const std::string &newPath) {
        std::ofstream os(newPath, std::ios::binary);
        cereal::BinaryOutputArchive archive(os);
        archive(simplifiedMesh);
    }

    void MeshImporter::SimplifyMesh(std::vector<std::string> &paths, const std::string &fileId,
                                    const std::string &targetDir,
                                    const MeshData &mesh, const LevelOfDetail &levelOfDetail) {
        size_t vertexCount = mesh.data.size();
        size_t indexCount = mesh.indices.size();
        size_t targetIndexCount = indexCount / levelOfDetail.level;

        // Prepare buffers for the simplified mesh
        std::vector<uint32_t> simplifiedIndices(indexCount);
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

        // Create a new mesh with simplified data
        MeshData simplifiedMesh;
        simplifiedMesh.data = mesh.data; // Keep the same vertices (positions, normals, UVs)
        simplifiedMesh.indices = std::move(simplifiedIndices);

        const std::string newPath = targetDir + "/" +
                                    LevelOfDetail::GetFormattedName(fileId, levelOfDetail, EntryType::MESH);
        paths.push_back(newPath);

        serializeMesh(simplifiedMesh, newPath);
    }

    std::vector<std::string> MeshImporter::getSupportedTypes() {
        return {"glb", "gltf", "fbx", "obj"};
    }
} // Metal
