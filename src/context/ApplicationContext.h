#ifndef METAL_ENGINE_APPLICATIONCONTEXT_H
#define METAL_ENGINE_APPLICATIONCONTEXT_H
#define CACHED_PATH "/metal-engine-cached.txt"
#include <string>

#include "engine/EngineContext.h"
#include "glfw/GLFWContext.h"
#include "vulkan/VulkanContext.h"
#include "../repository/framebuffer/CoreFrameBuffers.h"
#include "../repository/buffers/CoreBuffers.h"
#include "../repository/descriptors/CoreDescriptorSets.h"
#include "../repository/texture/CoreTextures.h"

#include "../service/mesh/MeshService.h"
#include "../service/world/WorldGridService.h"
#include "../service/texture/TextureService.h"
#include "../service/framebuffer/FrameBufferService.h"
#include "../service/pipeline/PipelineService.h"
#include "../service/buffer/BufferService.h"
#include "../service/descriptor/DescriptorService.h"
#include "../service/theme/ThemeService.h"
#include "../service/dock/DockService.h"
#include "../service/selection/SelectionService.h"
#include "../service/mesh/SceneImporterService.h"
#include "../service/texture/TextureImporterService.h"
#include "../service/files/FilesService.h"
#include "../service/camera/CameraService.h"
#include "./engine/passes/PassesService.h"

#include "../repository/world/impl/WorldGridRepository.h"
#include "../repository/inspection/FileInspectionRepository.h"
#include "../repository/world/WorldRepository.h"
#include "../repository/runtime/RuntimeRepository.h"
#include "../repository/streaming/StreamingRepository.h"
#include "../repository/engine/EngineRepository.h"
#include "../repository/dock/DockRepository.h"
#include "../repository/editor/EditorRepository.h"
#include "../service/material/MaterialService.h"
#include "../service/notification/NotificationService.h"
#include "../service/transform/TransformService.h"
#include "../service/voxel/SVOService.h"
#include "../service/lights/LightsService.h"
#include "../service/volumes/VolumesService.h"
#include "../service/voxel/VoxelizationService.h"
#include "../service/voxel/impl/VoxelizerService.h"
#include "editor/EditorPanel.h"
#include "gui/GuiContext.h"

namespace Metal {
    class ApplicationContext {
        bool debugMode;
        EditorPanel editorPanel;
        std::string rootDirectory;

    public:
        EngineContext engineContext{*this};
        PassesService passesService{*this};
        VulkanContext vulkanContext{*this, debugMode};
        GuiContext guiContext{*this};
        GLFWContext glfwContext{*this};

        // // ----------- CORE REPOSITORIES
        CoreFrameBuffers coreFrameBuffers{*this};
        CoreBuffers coreBuffers{*this};
        CoreDescriptorSets coreDescriptorSets{*this};
        CoreTextures coreTextures{*this};
        // ----------- CORE REPOSITORIES

        // ----------- Services
        NotificationService notificationService;
        MeshService meshService{*this};
        MaterialService materialService{*this};
        TextureService textureService{*this};
        FrameBufferService framebufferService{*this};
        PipelineService pipelineService{*this};
        BufferService bufferService{*this};
        DescriptorService descriptorService{*this};
        ThemeService themeService{*this};
        DockService dockService{*this};
        SelectionService selectionService{*this};
        SceneImporterService meshImporter{*this};
        TextureImporterService textureImporter{*this};
        FilesService filesService{*this};
        CameraService cameraService{*this};
        VoxelizationService voxelizationService{*this};
        VoxelizerService voxelizerService{*this};
        SVOService svoService{*this};
        WorldGridService worldGridService{*this};
        TransformService transformService{*this};
        LightsService lightsService{*this};
        VolumesService volumesService{*this};
        // ----------- Services

        // ----------- Repository
        FileInspectionRepository fileInspection{};
        WorldGridRepository worldGridRepository{*this};
        WorldRepository worldRepository{*this};
        RuntimeRepository runtimeRepository{};
        StreamingRepository streamingRepository{*this};
        EngineRepository engineRepository{};
        DockRepository dockRepository{*this};
        EditorRepository editorRepository{};
        // ----------- Repository

        [[nodiscard]] bool isDebugMode() const { return debugMode; }

        void updateRootPath(bool forceSelection);

        [[nodiscard]] bool isValidContext() const {
            return glfwContext.isValidContext();
        }

        [[nodiscard]] const std::string &getRootDirectory() const {
            return rootDirectory;
        }

        [[nodiscard]] std::string getAssetRefDirectory() const {
            return rootDirectory + "/assets-ref/";
        }

        [[nodiscard]] std::string getShadersDirectory() const {
            return rootDirectory + "/shaders/";
        }

        [[nodiscard]] std::string getAssetDirectory() const {
            return rootDirectory + "/assets/";
        }

        [[nodiscard]] uint32_t getFrameIndex() const;

        void start();

        void save();

        explicit ApplicationContext(const bool debugMode): debugMode(debugMode) {
            editorPanel.setContext(this);
        }
    };
}

#endif
