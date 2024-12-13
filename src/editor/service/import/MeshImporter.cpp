#include "MeshImporter.h"

#include "../../../context/runtime/MeshData.h"
#include <iostream>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "FileMetadata.h"
#include "../../../common/util/files/FilesUtil.h"
#include "meshoptimizer.h"
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
            aiMesh *mesh = scene->mMeshes[i];
            MeshData meshData;
            meshData.name = mesh->mName.data;

            for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
                aiVector3D vertex = mesh->mVertices[j];
                meshData.vertices.emplace_back(vertex.x, vertex.y, vertex.z);
            }

            for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
                aiFace face = mesh->mFaces[j];
                for (unsigned int k = 0; k < face.mNumIndices; ++k) {
                    meshData.indices.push_back(face.mIndices[k]);
                }
            }

            if (mesh->HasNormals()) {
                for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
                    aiVector3D normal = mesh->mNormals[j];
                    meshData.normals.emplace_back(normal.x, normal.y, normal.z);
                }
            }

            if (mesh->HasTextureCoords(0)) {
                for (unsigned int j = 0; j < mesh->mNumVertices; ++j) {
                    aiVector3D uv = mesh->mTextureCoords[0][j];
                    meshData.uvs.emplace_back(uv.x, uv.y);
                }
            }
            meshList.push_back(meshData);
        }

        return meshList;
    }

    void MeshImporter::importMesh(const std::string &targetDir, const std::string &pathToFile) {
        auto metadata = FileMetadata{};

        auto imported = ImportMeshes(pathToFile);
        for (MeshData &mesh: imported) {
            simplifyMesh(metadata.associatedFiles, metadata.getId(), targetDir, mesh, LevelOfDetail::LOD_0);
            simplifyMesh(metadata.associatedFiles, metadata.getId(), targetDir, mesh, LevelOfDetail::LOD_1);
            simplifyMesh(metadata.associatedFiles, metadata.getId(), targetDir, mesh, LevelOfDetail::LOD_2);
            simplifyMesh(metadata.associatedFiles, metadata.getId(), targetDir, mesh, LevelOfDetail::LOD_3);
        }
    }

    void MeshImporter::simplifyMesh(std::vector<std::string> &paths, const std::string &fileId,
                                    const std::string &targetDir,
                                    const MeshData &mesh, const LevelOfDetail &levelOfDetail) {
        MeshData lowerRes;
        lowerRes.name = mesh.name;
        lowerRes.normals = mesh.normals;
        lowerRes.vertices = mesh.vertices;
        lowerRes.indices = mesh.indices;
        lowerRes.uvs = mesh.uvs;
        // TODO

        // meshoptimizer::simplify_mesh(vertices, indices, (lowerRes.indices.size() / 3) / lod.level);
        const std::string newPath = targetDir + "/" +
                                    LevelOfDetail::GetFormattedName(fileId, levelOfDetail, EntryType::MESH);
        lowerRes.serialize(newPath);
    }
} // Metal
