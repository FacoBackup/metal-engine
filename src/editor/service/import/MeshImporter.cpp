#include "MeshImporter.h"


#include "../../../common/util/files/FileEntry.h"
#include "../../../context/runtime/MeshData.h"
#include "../../../context/runtime/VertexData.h"
#include <iostream>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "../../../common/util/files/FileMetadata.h"
#include "../../../engine/LevelOfDetail.h"

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
                    assimpMesh->mVertices[j].y,
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

        auto imported = ImportMeshes(pathToFile);
        for (MeshData &mesh: imported) {
            SimplifyMesh(metadata.associatedFiles, metadata.getId(), targetDir, mesh, LevelOfDetail::LOD_0);
            SimplifyMesh(metadata.associatedFiles, metadata.getId(), targetDir, mesh, LevelOfDetail::LOD_1);
            SimplifyMesh(metadata.associatedFiles, metadata.getId(), targetDir, mesh, LevelOfDetail::LOD_2);
            SimplifyMesh(metadata.associatedFiles, metadata.getId(), targetDir, mesh, LevelOfDetail::LOD_3);
        }
    }

    void MeshImporter::SimplifyMesh(std::vector<std::string> &paths, const std::string &fileId,
                                    const std::string &targetDir,
                                    const MeshData &mesh, const LevelOfDetail &levelOfDetail) {
        // MeshData lowerRes;
        // lowerRes.name = mesh.name;
        // lowerRes.normals = mesh.normals;
        // lowerRes.vertices = mesh.vertices;
        // lowerRes.indices = mesh.indices;
        // lowerRes.uvs = mesh.uvs;
        // TODO

        // meshoptimizer::simplify_mesh(vertices, indices, (lowerRes.indices.size() / 3) / lod.level);
        const std::string newPath = targetDir + "/" +
                                    LevelOfDetail::GetFormattedName(fileId, levelOfDetail, EntryType::MESH);
        // lowerRes.serialize(newPath);
    }
} // Metal
