#include "SceneImporterService.h"

#include "../../dto/file/FSEntry.h"
#include "MeshData.h"
#include "EntityAssetData.h"
#include "SceneData.h"
#include "../material/MaterialImporterService.h"
#include <iostream>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "../../util/FilesUtil.h"
#include "../../dto/file/EntryMetadata.h"
#include "../../enum/engine-definitions.h"
#include "../../context/ApplicationContext.h"
#include "../../util/serialization-definitions.h"
#include "../../dto/file/SceneImportSettingsDTO.h"

namespace Metal {

    std::string SceneImporterService::importData(const std::string &targetDir, const std::string &pathToFile,
                                                 const std::shared_ptr<ImportSettingsDTO> &settings,
                                                 const std::stop_token &stopToken) {

        Assimp::Importer importer;

        unsigned int flags = aiProcess_GlobalScale | aiProcess_FindInstances | aiProcess_PreTransformVertices;
        auto sceneSettings = std::dynamic_pointer_cast<SceneImportSettingsDTO>(settings);
        if (sceneSettings) {
            if (sceneSettings->triangulate) flags |= aiProcess_Triangulate;
            if (sceneSettings->flipUVs) flags |= aiProcess_FlipUVs;
            if (sceneSettings->genSmoothNormals) flags |= aiProcess_GenSmoothNormals;
            if (sceneSettings->generateUVs) flags |= aiProcess_GenUVCoords | aiProcess_TransformUVCoords;

            importer.SetPropertyFloat(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0.0f);
            importer.SetPropertyFloat(AI_CONFIG_GLOBAL_SCALE_FACTOR_KEY, sceneSettings->scale);
        }

        const aiScene *scene = importer.ReadFile(pathToFile, flags);

        if (stopToken.stop_requested()) {
           throw std::runtime_error("Import cancelled");
        }

        if (!scene || !scene->HasMeshes()) {
            LOG_ERROR("Error loading scene: " + std::string(importer.GetErrorString()));
            throw std::runtime_error("Error loading scene: " + std::string(importer.GetErrorString()));
        }

        SceneData sceneData{};
        auto sceneMetadata = EntryMetadata{};
        sceneMetadata.type = EntryType::SCENE;
        sceneMetadata.name = sceneData.name =
                             scene->mName.length > 0 ? scene->mName.data : scene->mRootNode->mName.data;

        std::string sceneBlobPath = CTX.getAssetDirectory() + FORMAT_FILE_SCENE(sceneMetadata.getId());


        std::unordered_map<unsigned int, std::string> meshMap{};
        std::unordered_map<std::string, unsigned int> meshMaterialMap{};

        if (stopToken.stop_requested()) {
            throw std::runtime_error("Import cancelled");
        }

        CTX.meshImporterService.persistAllMeshes(targetDir, scene, meshMap, meshMaterialMap, stopToken);

        if (stopToken.stop_requested()) {
            throw std::runtime_error("Import cancelled");
        }

        std::unordered_map<unsigned int, MaterialData> materialsMap{};
        fs::path absolutePath = fs::absolute(pathToFile);
        fs::path directoryPath = absolutePath.parent_path();

        CTX.materialImporterService.collectMaterials(targetDir, scene, materialsMap, directoryPath.string(),
                                                            stopToken);

        if (stopToken.stop_requested()) {
            throw std::runtime_error("Import cancelled");
        }

        int increment = 0;
        ProcessNode(increment, sceneData, scene->mRootNode, -1, meshMap, meshMaterialMap, materialsMap, stopToken);

        if (stopToken.stop_requested()) {
            throw std::runtime_error("Import cancelled");
        }
        DUMP_TEMPLATE(sceneBlobPath, sceneData)
        sceneMetadata.size = fs::file_size(sceneBlobPath);
        DUMP_TEMPLATE(targetDir + '/' + FORMAT_FILE_METADATA(sceneMetadata.getId()), sceneMetadata)

        return sceneMetadata.getId();
    }

    void SceneImporterService::ProcessNode(int &increment, SceneData &scene, const aiNode *node, int parentId,
                                           const std::unordered_map<unsigned int, std::string> &meshMap,
                                           const std::unordered_map<std::string, unsigned int> &meshMaterialMap,
                                           const std::unordered_map<unsigned int, MaterialData> &materialsMap,
                                           const std::stop_token &stopToken) {
        if (stopToken.stop_requested()) return;
        auto &currentNode = scene.entities.emplace_back();

        currentNode.name = node->mName.data;
        currentNode.id = increment;
        increment++;
        currentNode.parentEntity = parentId;

        // IMPORTANT: `scene.entities` is a `std::vector`. Any further `emplace_back` may reallocate and
        // invalidate references (including `currentNode`). Capture the needed values by copy.
        const int currentNodeId = currentNode.id;
        const std::string currentNodeName = currentNode.name;

        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            if (stopToken.stop_requested()) return;
            unsigned int meshIndex = node->mMeshes[i];
            const auto it = meshMap.find(meshIndex);
            if (it == meshMap.end()) {
                // Mesh reference missing; skip linking to avoid terminating import.
                continue;
            }

            auto &childMeshNode = scene.entities.emplace_back();
            childMeshNode.meshId = it->second;
            childMeshNode.name = currentNodeName + " (" + std::to_string(meshIndex) + ")";
            childMeshNode.parentEntity = currentNodeId;
            childMeshNode.id = increment;
            if (meshMaterialMap.contains(childMeshNode.meshId)) {
                unsigned int matIndex = meshMaterialMap.at(childMeshNode.meshId);
                if (materialsMap.contains(matIndex)) {
                    const auto &matData = materialsMap.at(matIndex);
                    childMeshNode.albedo = matData.albedo;
                    childMeshNode.roughness = matData.roughness;
                    childMeshNode.metallic = matData.metallic;
                }
            }
            increment++;
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            if (stopToken.stop_requested()) return;
            ProcessNode(increment, scene, node->mChildren[i], currentNodeId, meshMap, meshMaterialMap, materialsMap,
                        stopToken);
        }
    }

    std::vector<std::string> SceneImporterService::getSupportedTypes() {
        return {"glb", "gltf", "fbx", "obj"};
    }
}
