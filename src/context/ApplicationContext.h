#ifndef METAL_ENGINE_APPLICATIONCONTEXT_H
#define METAL_ENGINE_APPLICATIONCONTEXT_H
#define CACHED_PATH "/metal-engine-cached.txt"
#include <string>

#include "engine/EngineContext.h"
#include "glfw/GLFWContext.h"
#include "vulkan/VulkanContext.h"
#include "../repository/framebuffer/CoreFrameBuffers.h"
#include "../repository/buffers/CoreBuffers.h"
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
#include "../service/mesh/MeshImporterService.h"
#include "../service/material/MaterialImporterService.h"
#include "../service/texture/TextureImporterService.h"
#include "../service/files/FilesService.h"
#include "../service/files/FileImporterService.h"
#include "../service/camera/CameraService.h"
#include "./engine/passes/PassesService.h"

#include "../repository/world/impl/WorldGridRepository.h"
#include "../repository/inspection/FilesRepository.h"
#include "../repository/world/WorldRepository.h"
#include "../repository/runtime/RuntimeRepository.h"
#include "../repository/streaming/StreamingService.h"
#include "../repository/engine/EngineRepository.h"
#include "../repository/dock/DockRepository.h"
#include "../repository/editor/EditorRepository.h"
#include "../service/material/MaterialService.h"
#include "../service/notification/NotificationService.h"
#include "../service/notification/AsyncTaskService.h"
#include "../service/log/LogService.h"
#include "../service/transform/TransformService.h"
#include "../service/picking/PickingService.h"
#include "../service/volumes/VolumeService.h"
#include "../service/voxel/VoxelImporterService.h"
#include "../service/voxel/VoxelService.h"
#include "../service/lights/LightService.h"
#include "../service/raytracing/RayTracingService.h"
#include "editor/EditorPanel.h"
#include "gui/GuiContext.h"
#define CTX Metal::ApplicationContext::Get()
namespace Metal {
    class ApplicationContext {
        static std::unique_ptr<ApplicationContext> CONTEXT;
        bool debugMode;
        EditorPanel editorPanel;
        std::string rootDirectory;

    public:
        explicit ApplicationContext(bool debug_mode)
            : debugMode(debug_mode) {
        }

        EngineContext engineContext{};
        PassesService passesService{};
        VulkanContext vulkanContext{ debugMode};
        GuiContext guiContext{};
        GLFWContext glfwContext{};

        // // ----------- CORE REPOSITORIES
        CoreFrameBuffers coreFrameBuffers{};
        CoreBuffers coreBuffers{};
        CoreTextures coreTextures{};
        // ----------- CORE REPOSITORIES

        // ----------- Services
        NotificationService notificationService;
        AsyncTaskService asyncTaskService;
        LogService logService{};
        MeshService meshService{};
        MaterialService materialService{};
        TextureService textureService{};
        FrameBufferService framebufferService{};
        PipelineService pipelineService{};
        BufferService bufferService{};
        DescriptorService descriptorService{};
        ThemeService themeService{};
        DockService dockService{};
        SelectionService selectionService{};
        SceneImporterService sceneImporterService{};
        MeshImporterService meshImporterService{};
        MaterialImporterService materialImporterService{};
        TextureImporterService textureImporter{};
        FilesService filesService{};
        FileImporterService fileImporterService{};
        CameraService cameraService{};
        PickingService pickingService{};
        WorldGridService worldGridService{};
        TransformService transformService{};
        LightService lightService{};
        VolumeService volumeService{};
        RayTracingService rayTracingService{};
        VoxelImporterService voxelImporterService{};
        VoxelService voxelService{};
        // ----------- Services

        // ----------- Repository
        FilesRepository fileInspection{};
        WorldGridRepository worldGridRepository{};
        WorldRepository worldRepository{};
        RuntimeRepository runtimeRepository{};
        StreamingService streamingRepository{};
        EngineRepository engineRepository{};
        DockRepository dockRepository{};
        EditorRepository editorRepository{};
        // ----------- Repository


        static ApplicationContext &Get();

        static void Init(bool debugMode);

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

        [[nodiscard]] std::string getVideoOutputPath() const {
            return rootDirectory + "/output.mp4";
        }

        [[nodiscard]] unsigned int getFrameIndex() const;

        void dispose();

        void start();

        void save();
    };
}

#endif
