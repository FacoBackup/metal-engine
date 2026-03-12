#include "SceneImporterService.h"

#include "../../dto/file/FSEntry.h"
#include "MeshData.h"
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
        if (auto sceneSettings = std::dynamic_pointer_cast<SceneImportSettingsDTO>(settings)) {
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

        std::unordered_map<unsigned int, MeshId> meshMap{};

        if (stopToken.stop_requested()) {
            throw std::runtime_error("Import cancelled");
        }

        CTX.meshImporterService.persistAllMeshes(targetDir, scene, meshMap, stopToken);

        if (stopToken.stop_requested()) {
            throw std::runtime_error("Import cancelled");
        }

        fs::path absolutePath = fs::absolute(pathToFile);
        std::string directoryPath = absolutePath.parent_path().string();

        ProcessNode(sceneData, scene, scene->mRootNode, targetDir, directoryPath, meshMap, stopToken);
        ProcessLights(sceneData, scene);

        if (stopToken.stop_requested()) {
            throw std::runtime_error("Import cancelled");
        }

        std::vector<SceneEntityData> entities{};
        for (SceneEntityData &entity: sceneData.entities) {
            if (!entity.primitive) {
                continue;
            }
            entities.push_back(entity);
        }
        sceneData.entities = entities;
        DUMP_TEMPLATE(sceneBlobPath, sceneData)
        sceneMetadata.size = fs::file_size(sceneBlobPath);
        DUMP_TEMPLATE(targetDir + '/' + FORMAT_FILE_METADATA(sceneMetadata.getId()), sceneMetadata)

        return sceneMetadata.getId();
    }

    void SceneImporterService::ProcessNode(SceneData &scene, const aiScene *aiScene, const aiNode *node,
                                           const std::string &targetDir, const std::string &rootDirectory,
                                           const std::unordered_map<unsigned int, MeshId> &meshMap,
                                           const std::stop_token &stopToken) {
        if (stopToken.stop_requested()) return;
        auto &currentNode = scene.entities.emplace_back();

        currentNode.entity.name = node->mName.data;

        // Extract transform
        aiVector3D scaling, pos;
        aiQuaternion rotation;
        node->mTransformation.Decompose(scaling, rotation, pos);
        currentNode.transform.translation = {pos.x, pos.y, pos.z};
        currentNode.transform.rotation = {rotation.w, rotation.x, rotation.y, rotation.z};
        currentNode.transform.scale = {scaling.x, scaling.y, scaling.z};

        ProcessMeshes(scene, aiScene, node, targetDir, rootDirectory, meshMap, stopToken);

        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            ProcessNode(scene, aiScene, node->mChildren[i], targetDir, rootDirectory, meshMap, stopToken);
        }
    }

    void SceneImporterService::ProcessMeshes(SceneData &scene, const aiScene *aiScene, const aiNode *node,
                                             const std::string &targetDir, const std::string &rootDirectory,
                                             const std::unordered_map<unsigned int, MeshId> &meshMap,
                                             const std::stop_token &stopToken) {
        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            if (stopToken.stop_requested()) return;
            unsigned int meshIndex = node->mMeshes[i];
            const auto it = meshMap.find(meshIndex);
            if (it == meshMap.end()) continue;

            auto &childMeshNode = scene.entities.emplace_back();
            childMeshNode.primitive = PrimitiveComponent();
            childMeshNode.primitive->meshId = it->second.id;
            childMeshNode.transform.gizmoCenter = it->second.gizmoCenter;

            const aiMesh *assimpMesh = aiScene->mMeshes[meshIndex];
            childMeshNode.entity.name = assimpMesh->mName.length > 0 ? assimpMesh->mName.data : "Mesh";

            const aiMaterial *material = aiScene->mMaterials[assimpMesh->mMaterialIndex];
            CTX.materialImporterService.importMaterial(targetDir, material, aiScene, rootDirectory,
                                                       *childMeshNode.primitive, stopToken);
        }
    }

    void SceneImporterService::ProcessLights(SceneData &scene, const aiScene *aiScene) {
        for (unsigned int i = 0; i < aiScene->mNumLights; ++i) {
            aiLight *light = aiScene->mLights[i];
            auto &lightNode = scene.entities.emplace_back();
            lightNode.entity.name = light->mName.data;
            lightNode.transform.translation = {light->mPosition.x, light->mPosition.y, light->mPosition.z};
        }
    }

    std::vector<std::string> SceneImporterService::getSupportedTypes() {
        return {"glb", "gltf", "fbx", "obj"};
    }
}
