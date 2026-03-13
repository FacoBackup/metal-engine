#ifndef METAL_ENGINE_APPLICATIONCONTEXT_H
#define METAL_ENGINE_APPLICATIONCONTEXT_H
#define CACHED_PATH "/metal-engine-cached.txt"
#include <string>

#include "engine/EngineContext.h"
#include "core/glfw/GLFWContext.h"
#include "core/vulkan/VulkanContext.h"

#include "engine/service/MeshService.h"
#include "engine/service/TextureService.h"
#include "engine/service/FrameBufferService.h"
#include "engine/service/PipelineService.h"
#include "engine/service/BufferService.h"
#include "engine/service/DescriptorSetService.h"
#include "editor/service/ThemeService.h"
#include "editor/service/DockService.h"
#include "editor/service/SelectionService.h"
#include "editor/service/SceneImporterService.h"
#include "editor/service/MeshImporterService.h"
#include "editor/service/MaterialImporterService.h"
#include "editor/service/TextureImporterService.h"
#include "editor/service/FilesService.h"
#include "editor/service/FileImporterService.h"
#include "engine/service/CameraService.h"

#include "engine/repository/WorldRepository.h"
#include "engine/repository/RuntimeRepository.h"
#include "engine/service/StreamingService.h"
#include "engine/repository/EngineRepository.h"
#include "editor/repository/DockRepository.h"
#include "editor/repository/EditorRepository.h"
#include "engine/service/MaterialService.h"
#include "engine/service/CommandBufferRecorderService.h"
#include "editor/service/NotificationService.h"
#include "editor/service/AsyncTaskService.h"
#include "editor/service/LogService.h"
#include "engine/service/TransformService.h"
#include "editor/service/PickingService.h"
#include "engine/service/VolumeService.h"
#include "editor/service/VoxelImporterService.h"
#include "engine/service/VoxelService.h"
#include "engine/service/LightService.h"
#include "engine/service/RayTracingService.h"
#include "editor/EditorPanel.h"
#include "core/gui/GuiContext.h"

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
        VulkanContext vulkanContext{ debugMode};
        GuiContext guiContext{};
        GLFWContext glfwContext{};

        // ----------- Services
        NotificationService notificationService;
        AsyncTaskService asyncTaskService;
        LogService logService{};
        MeshService meshService{};
        MaterialService materialService{};
        TextureService textureService{};
        FrameBufferService framebufferService{};
        DescriptorSetService descriptorSetService{};
        PipelineService pipelineService{descriptorSetService};
        BufferService bufferService{};
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
        TransformService transformService{};
        LightService lightService{};
        VolumeService volumeService{};
        RayTracingService rayTracingService{};
        CommandBufferRecorderService commandBufferRecorderService{};
        VoxelImporterService voxelImporterService{};
        VoxelService voxelService{};
        // ----------- Services

        // ----------- Repository
        WorldRepository worldRepository{};
        RuntimeRepository runtimeRepository{};
        StreamingService streamingService{};
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
