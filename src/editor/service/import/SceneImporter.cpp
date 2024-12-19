#include "SceneImporter.h"


#include "../../../common/util/files/FileEntry.h"
#include "../../../context/runtime/assets/MeshData.h"
#include "../../../context/runtime/assets/EntityAssetData.h"
#include "../../../context/runtime/assets/VertexData.h"
#include "../../../context/runtime/assets/SceneData.h"
#include <iostream>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "../../../common/util/files/FilesUtil.h"
#include "../../../common/util/files/FileMetadata.h"
#include "../../../engine/engine-definitions.h"
#include "../../../engine/enum/LevelOfDetail.h"
#include <meshoptimizer.h>
#include <cereal/archives/binary.hpp>
#include <utility>

#include "../../../context/ApplicationContext.h"

namespace Metal {
    std::string SceneImporter::persistMesh(const std::string &targetDir, const std::string &pathToFile,
                                           const MeshData &mesh) const {
        auto metadata = FileMetadata{};
        metadata.type = EntryType::MESH;
        metadata.name = std::filesystem::path(pathToFile).filename().string();
        metadata.name = metadata.name.substr(0, metadata.name.find_last_of('.'));
        FilesUtil::WriteFile((targetDir + '/' + metadata.getId() + FILE_METADATA).c_str(),
                             metadata.serialize().c_str());

        serializeMesh(mesh, context.getAssetDirectory() + FORMAT_FILE_MESH(metadata.getId(), LevelOfDetail::LOD_0));
        simplifyMesh(metadata.getId(), mesh, LevelOfDetail::LOD_1);
        simplifyMesh(metadata.getId(), mesh, LevelOfDetail::LOD_2);
        simplifyMesh(metadata.getId(), mesh, LevelOfDetail::LOD_3);
        return metadata.getId();
    }

    void SceneImporter::persistAllMeshes(const std::string &targetDir, const std::string &pathToFile,
                                         const aiScene *scene,
                                         std::unordered_map<unsigned int, std::string> &meshMap) const {
        for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
            aiMesh *assimpMesh = scene->mMeshes[i];
            MeshData meshData{assimpMesh->mName.data, {}, {}};
            for (unsigned int j = 0; j < assimpMesh->mNumVertices; ++j) {
                VertexData vertexData{};
                vertexData.vertex = glm::vec3(
                    assimpMesh->mVertices[j].x,
                    -assimpMesh->mVertices[j].y,
                    assimpMesh->mVertices[j].z
                );
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
            for (unsigned int j = 0; j < assimpMesh->mNumFaces; ++j) {
                aiFace face = assimpMesh->mFaces[j];
                for (unsigned int k = 0; k < face.mNumIndices; ++k) {
                    meshData.indices.push_back(face.mIndices[k]);
                }
            }
            meshMap.insert({i, persistMesh(targetDir, pathToFile, meshData)});
        }
    }

    void SceneImporter::importScene(const std::string &targetDir, const std::string &pathToFile) const {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(
            pathToFile,
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GlobalScale | aiProcess_FindInstances |
            aiProcess_PreTransformVertices | aiProcess_GenSmoothNormals);

        if (!scene || !scene->HasMeshes()) {
            std::cerr << "Error loading scene: " << importer.GetErrorString() << std::endl;
            return;
        }

        SceneData sceneData{};
        auto sceneMetadata = FileMetadata{};
        sceneMetadata.type = EntryType::SCENE;
        sceneMetadata.name = sceneData.name =
                             scene->mName.length > 0 ? scene->mName.data : scene->mRootNode->mName.data;
        FilesUtil::WriteFile((targetDir + '/' + FORMAT_FILE_METADATA(sceneMetadata.getId())).c_str(),
                             sceneMetadata.serialize().c_str());

        std::unordered_map<unsigned int, std::string> meshMap{};
        persistAllMeshes(targetDir, pathToFile, scene, meshMap);
        int increment = 0;
        ProcessNode(increment, sceneData, scene->mRootNode, -1, meshMap);

        std::ofstream os(context.getAssetDirectory() + FORMAT_FILE_SCENE(sceneMetadata.getId()), std::ios::binary);
        cereal::BinaryOutputArchive archive(os);
        archive(sceneData);
    }

    void SceneImporter::ProcessNode(int &increment, SceneData &scene, const aiNode *node, const int parentId,
                                    std::unordered_map<unsigned int, std::string> &meshMap) {
        auto &currentNode = scene.entities.emplace_back();

        aiVector3D translation, scale;
        aiQuaternion rotation;
        node->mTransformation.Decompose(scale, rotation, translation);
        currentNode.scale = {scale.x, scale.y, scale.z};
        currentNode.position = {translation.x, translation.y, translation.z};
        currentNode.rotation = {rotation.x, rotation.y, rotation.z, rotation.w};
        currentNode.name = node->mName.data;
        currentNode.id = increment;
        increment++;
        currentNode.parentEntity = parentId;

        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            auto &childMeshNode = scene.entities.emplace_back();
            unsigned int meshIndex = node->mMeshes[i];
            childMeshNode.meshId = meshMap.at(meshIndex);
            childMeshNode.name = currentNode.name + " (" + std::to_string(meshIndex) + ")";
            childMeshNode.parentEntity = currentNode.id;
            childMeshNode.id = increment;
            increment++;
        }

        // Recursively process child nodes
        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            ProcessNode(increment, scene, node->mChildren[i], currentNode.id, meshMap);
        }
    }

    void SceneImporter::serializeMesh(const MeshData &simplifiedMesh, const std::string &newPath) {
        std::ofstream os(newPath, std::ios::binary);
        cereal::BinaryOutputArchive archive(os);
        archive(simplifiedMesh);
    }

    void SceneImporter::simplifyMesh(const std::string &fileId, const MeshData &mesh,
                                     const LevelOfDetail &levelOfDetail) const {
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

        serializeMesh(simplifiedMesh, context.getAssetDirectory() + FORMAT_FILE_MESH(fileId, levelOfDetail));
    }

    std::vector<std::string> SceneImporter::getSupportedTypes() {
        return {"glb", "gltf", "fbx", "obj"};
    }
} // Metal