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
#include "engine/service/TransformService.h"
#include "editor/service/PickingService.h"
#include "engine/service/VolumeService.h"
#include "editor/service/VoxelImporterService.h"
#include "engine/service/VoxelService.h"
#include "engine/service/LightService.h"
#include "engine/service/RayTracingService.h"
#include "editor/EditorPanel.h"
#include "core/gui/GuiContext.h"

#define CTX ApplicationContext::CONTEXT
namespace Metal {
    class ApplicationContext : public Initializable{
        EditorPanel editorPanel;
        std::string rootDirectory;

    public:
        static ApplicationContext CONTEXT;

        VulkanContext vulkanContext{
            true,
            glfwContext,
            engineRepository,
            meshService,
            textureService,
            framebufferService,
            pipelineService,
            rayTracingService
        };
        GuiContext guiContext{vulkanContext, glfwContext, descriptorSetService};
        GLFWContext glfwContext{};

        EngineContext engineContext{
            transformService,
            streamingService,
            rayTracingService,
            cameraService,
            lightService,
            volumeService,
            worldRepository,
            editorRepository,
            engineRepository
        };

        // ----------- Repository
        WorldRepository worldRepository{rayTracingService, rootDirectory};
        RuntimeRepository runtimeRepository{};
        EngineRepository engineRepository{};
        DockRepository dockRepository{};
        EditorRepository editorRepository{};

        // ----------- Services
        NotificationService notificationService;
        AsyncTaskService asyncTaskService;
        BufferService bufferService{vulkanContext};
        FrameBufferService framebufferService{vulkanContext};
        ThemeService themeService{editorRepository};
        DockService dockService{dockRepository};
        DescriptorSetService descriptorSetService{vulkanContext, framebufferService, bufferService, textureService};
        SelectionService selectionService{editorRepository, worldRepository};
        VoxelImporterService voxelImporterService{rootDirectory};
        MeshImporterService meshImporterService{rootDirectory};
        TextureImporterService textureImporter{rootDirectory};
        MaterialImporterService materialImporterService{textureImporter, rootDirectory};
        SceneImporterService sceneImporterService{meshImporterService, materialImporterService, rootDirectory};
        FilesService filesService{rootDirectory, notificationService};
        FileImporterService fileImporterService{
            sceneImporterService, textureImporter, voxelImporterService, notificationService, asyncTaskService
        };
        CameraService cameraService{engineContext, worldRepository, runtimeRepository};
        PickingService pickingService{vulkanContext, bufferService, worldRepository};
        TransformService transformService{worldRepository, rayTracingService};
        LightService lightService{engineContext, engineRepository};
        VolumeService volumeService{worldRepository, engineContext};
        RayTracingService rayTracingService{
            vulkanContext, pipelineService, worldRepository, meshService, materialService, bufferService, engineContext
        };
        CommandBufferRecorderService commandBufferRecorderService{};
        VoxelService voxelService{};
        StreamingService streamingService{engineContext, worldRepository, meshService, textureService, voxelService};
        // ----------- Services
        // ----------- Repository

        [[nodiscard]] bool isDebugMode() const { return true; }

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
