#include "SceneImporterService.h"


#include "../../dto/file/FileEntry.h"
#include "MeshData.h"
#include "EntityAssetData.h"
#include "VertexData.h"
#include "SceneData.h"
#include <iostream>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "../../util/FilesUtil.h"
#include "../../dto/file/FileMetadata.h"
#include "../../dto/buffers/MaterialInfo.h"
#include "../../enum/engine-definitions.h"
#include "../../enum/LevelOfDetail.h"
#include <meshoptimizer.h>
#include <unordered_map>
#include <../../dependencies/stb/stb_image_write.h>
#include <cereal/archives/binary.hpp>

#include "../../context/ApplicationContext.h"

#define TEXTURE_P context.textureImporter.importTexture(targetDir, rootDirectory + "/" + texturePath.data)

namespace Metal {
    std::string SceneImporterService::persistMesh(const std::string &targetDir, const MeshData &mesh) const {
        auto metadata = FileMetadata{};
        metadata.type = EntryType::MESH;
        metadata.name = mesh.name;
        metadata.name = metadata.name.substr(0, metadata.name.find_last_of('.'));
        DUMP_TEMPLATE(targetDir + '/' + metadata.getId() + FILE_METADATA, metadata)
        DUMP_TEMPLATE(context.getAssetDirectory() + FORMAT_FILE_MESH(metadata.getId(), LevelOfDetail::LOD_0), mesh)
        simplifyMesh(metadata.getId(), mesh, LevelOfDetail::LOD_1);
        simplifyMesh(metadata.getId(), mesh, LevelOfDetail::LOD_2);
        simplifyMesh(metadata.getId(), mesh, LevelOfDetail::LOD_3);
        return metadata.getId();
    }

    void SceneImporterService::persistAllMeshes(const std::string &targetDir, const aiScene *scene,
                                                std::unordered_map<unsigned int, std::string> &meshMap) const {
        for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
            aiMesh *assimpMesh = scene->mMeshes[i];
            MeshData meshData{assimpMesh->mName.data, {}, {}};
            for (unsigned int j = 0; j < assimpMesh->mNumVertices; ++j) {
                VertexData vertexData{};
                vertexData.vertex = glm::vec3(
                    assimpMesh->mVertices[j].x,
                    assimpMesh->mVertices[j].y,
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
            std::string id = persistMesh(targetDir, meshData);
            meshMap.insert({i, id});
        }
    }

    void SceneImporterService::importScene(const std::string &targetDir, const std::string &pathToFile) const {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(
            pathToFile,
            aiProcess_Triangulate | aiProcess_GlobalScale |
            aiProcess_FindInstances | aiProcess_FlipUVs | aiProcess_TransformUVCoords | aiProcess_GenUVCoords |
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
        DUMP_TEMPLATE(targetDir + '/' + FORMAT_FILE_METADATA(sceneMetadata.getId()), sceneMetadata)

        std::unordered_map<unsigned int, std::string> meshMap{};
        persistAllMeshes(targetDir, scene, meshMap);
        fs::path absolutePath = fs::absolute(pathToFile);
        fs::path directoryPath = absolutePath.parent_path(); // Get the directory

        int increment = 0;
        ProcessNode(increment, sceneData, scene->mRootNode, -1, meshMap);

        DUMP_TEMPLATE(context.getAssetDirectory() + FORMAT_FILE_SCENE(sceneMetadata.getId()), sceneData)
    }

    void SceneImporterService::ProcessNode(int &increment, SceneData &scene, const aiNode *node, int parentId,
                                           const std::unordered_map<unsigned int, std::string> &meshMap) {
        auto &currentNode = scene.entities.emplace_back();

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

    void SceneImporterService::simplifyMesh(const std::string &fileId, const MeshData &mesh,
                                            const LevelOfDetail &levelOfDetail) const {
        size_t vertexCount = mesh.data.size();
        size_t indexCount = mesh.indices.size();
        size_t targetIndexCount = indexCount / levelOfDetail.level;

        // Prepare buffers for the simplified mesh
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

        // Create a new mesh with simplified data
        MeshData simplifiedMesh;
        simplifiedMesh.data = mesh.data; // Keep the same vertices (positions, normals, UVs)
        simplifiedMesh.indices = std::move(simplifiedIndices);

        DUMP_TEMPLATE(context.getAssetDirectory() + FORMAT_FILE_MESH(fileId, levelOfDetail), simplifiedMesh)
    }

    std::vector<std::string> SceneImporterService::getSupportedTypes() {
        return {"glb", "gltf", "fbx", "obj"};
    }
} // Metal
