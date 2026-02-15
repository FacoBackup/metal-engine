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
#include "../../enum/engine-definitions.h"
#include "../../enum/LevelOfDetail.h"
#include "../../context/ApplicationContext.h"
#include "../material/MaterialData.h"
#include <future>

#include <cereal/archives/binary.hpp>

namespace Metal {
    void SceneImporterService::importScene(const std::string &targetDir, const std::string &pathToFile) const {
        LOG_INFO(context, "Starting async scene import: " + pathToFile);

        std::stop_source stopSource;
        std::string task = context.asyncTaskService.registerTask("Importing scene: " + fs::path(pathToFile).filename().string(),
            [stopSource]() mutable {
                stopSource.request_stop();
            });

        std::jthread([this, targetDir, pathToFile, task, stopToken = stopSource.get_token()]() {
            Assimp::Importer importer;
            const aiScene *scene = importer.ReadFile(
                pathToFile,
                aiProcess_Triangulate | aiProcess_GlobalScale |
                aiProcess_FindInstances | aiProcess_FlipUVs | aiProcess_TransformUVCoords | aiProcess_GenUVCoords |
                aiProcess_PreTransformVertices | aiProcess_GenSmoothNormals);

            if (stopToken.stop_requested()) {
                context.asyncTaskService.endTask(task);
                return;
            }

            if (!scene || !scene->HasMeshes()) {
                LOG_ERROR(context, "Error loading scene: " + std::string(importer.GetErrorString()));
                context.asyncTaskService.endTask(task);
                return;
            }

            SceneData sceneData{};
            auto sceneMetadata = FileMetadata{};
            sceneMetadata.type = EntryType::SCENE;
            sceneMetadata.name = sceneData.name =
                                 scene->mName.length > 0 ? scene->mName.data : scene->mRootNode->mName.data;
            DUMP_TEMPLATE(targetDir + '/' + FORMAT_FILE_METADATA(sceneMetadata.getId()), sceneMetadata)

            std::unordered_map<unsigned int, std::string> meshMap{};
            std::unordered_map<std::string, unsigned int> meshMaterialMap{};

            if (stopToken.stop_requested()) {
                context.asyncTaskService.endTask(task);
                return;
            }

            context.meshImporterService.persistAllMeshes(targetDir, scene, meshMap, meshMaterialMap, stopToken);

            if (stopToken.stop_requested()) {
                context.asyncTaskService.endTask(task);
                return;
            }

            std::unordered_map<unsigned int, std::string> materialsMap{};
            fs::path absolutePath = fs::absolute(pathToFile);
            fs::path directoryPath = absolutePath.parent_path();

            context.materialImporterService.persistAllMaterials(targetDir, scene, materialsMap, directoryPath.string(), stopToken);

            if (stopToken.stop_requested()) {
                context.asyncTaskService.endTask(task);
                return;
            }

            int increment = 0;
            ProcessNode(increment, sceneData, scene->mRootNode, -1, meshMap, meshMaterialMap, materialsMap, stopToken);

            if (stopToken.stop_requested()) {
                context.asyncTaskService.endTask(task);
                return;
            }

            DUMP_TEMPLATE(context.getAssetDirectory() + FORMAT_FILE_SCENE(sceneMetadata.getId()), sceneData)
            LOG_INFO(context, "Successfully imported scene: " + sceneMetadata.name + " (" + sceneMetadata.getId() + ")");
            context.asyncTaskService.endTask(task);
        }).detach();
    }

    void SceneImporterService::ProcessNode(int &increment, SceneData &scene, const aiNode *node, int parentId,
                                           const std::unordered_map<unsigned int, std::string> &meshMap,
                                           const std::unordered_map<std::string, unsigned int> &meshMaterialMap,
                                           const std::unordered_map<unsigned int, std::string> &materialsMap,
                                           const std::stop_token &stopToken) {
        if (stopToken.stop_requested()) return;
        auto &currentNode = scene.entities.emplace_back();

        currentNode.name = node->mName.data;
        currentNode.id = increment;
        increment++;
        currentNode.parentEntity = parentId;

        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            if (stopToken.stop_requested()) return;
            auto &childMeshNode = scene.entities.emplace_back();
            unsigned int meshIndex = node->mMeshes[i];
            childMeshNode.meshId = meshMap.at(meshIndex);
            childMeshNode.name = currentNode.name + " (" + std::to_string(meshIndex) + ")";
            childMeshNode.parentEntity = currentNode.id;
            childMeshNode.id = increment;
            if (meshMaterialMap.contains(childMeshNode.meshId)) {
                unsigned int matIndex = meshMaterialMap.at(childMeshNode.meshId);
                if (materialsMap.contains(matIndex)) {
                    childMeshNode.materialId = materialsMap.at(matIndex);
                }
            }
            increment++;
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            if (stopToken.stop_requested()) return;
            ProcessNode(increment, scene, node->mChildren[i], currentNode.id, meshMap, meshMaterialMap, materialsMap, stopToken);
        }
    }

    std::vector<std::string> SceneImporterService::getSupportedTypes() {
        return {"glb", "gltf", "fbx", "obj"};
    }
}
