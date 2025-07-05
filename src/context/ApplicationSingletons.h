#ifndef APPLICATIONSINGLETONS_H
#define APPLICATIONSINGLETONS_H

#include "../runtime/EngineContext.h"
#include "../ui/GLFWContext.h"
#include "../renderer/VulkanContext.h"
#include "../renderer/repository/CoreFrameBuffers.h"
#include "../renderer/repository/CoreBuffers.h"
#include "../renderer/repository/CoreDescriptorSets.h"
#include "../renderer/repository/CoreTextures.h"

#include "../renderer/service/BVHBuilderService.h"
#include "../renderer/service/MeshService.h"
#include "../renderer/service/TextureService.h"
#include "../renderer/service/FrameBufferService.h"
#include "../renderer/service/WindowService.h"
#include "../renderer/service/PipelineService.h"
#include "../renderer/service/BufferService.h"
#include "../renderer/service/DescriptorService.h"
#include "../ui/service/ThemeService.h"
#include "../ui/service/SelectionService.h"
#include "../renderer/service/SceneImporterService.h"
#include "../renderer/service/TextureImporterService.h"
#include "../ui/service/FilesService.h"
#include "../runtime/service/CameraService.h"
#include "../renderer/service/PassesService.h"

#include "../ui/repository/FileInspectionRepository.h"
#include "../runtime/service/WorldRepository.h"
#include "../runtime/repository/RuntimeRepository.h"
#include "../renderer/service/StreamingService.h"
#include "../runtime/repository/EngineRepository.h"
#include "../ui/repository/EditorRepository.h"
#include "../renderer/service/MaterialService.h"
#include "../ui/service/NotificationService.h"
#include "../runtime/service/TransformService.h"
#include "../renderer/service/LightsService.h"
#include "../renderer/service/VolumeService.h"
#include "../ui/editor/EditorPanel.h"
#include "../ui/GuiContext.h"

namespace Metal
{
    struct ApplicationSingletons
    {
        EditorPanel editorPanel;

        EngineContext engineContext{};
        PassesService passesService{};
        VulkanContext vulkanContext{};
        GuiContext guiContext{};
        GLFWContext glfwContext{};

        // // ----------- CORE REPOSITORIES
        CoreFrameBuffers coreFrameBuffers{};
        CoreBuffers coreBuffers{};
        CoreDescriptorSets coreDescriptorSets{};
        CoreTextures coreTextures{};
        // ----------- CORE REPOSITORIES

        // ----------- Services
        NotificationService notificationService;
        MeshService meshService{};
        MaterialService materialService{};
        TextureService textureService{};
        FrameBufferService framebufferService{};
        PipelineService pipelineService{};
        BufferService bufferService{};
        DescriptorService descriptorService{};
        ThemeService themeService{};
        SelectionService selectionService{};
        SceneImporterService meshImporter{};
        TextureImporterService textureImporter{};
        FilesService filesService{};
        CameraService cameraService{};
        TransformService transformService{};
        LightsService lightsService{};
        VolumeService volumesService{};
        BVHBuilderService bvhBuilderService{};
        WindowService windowService{};
        // ----------- Services

        // ----------- Repository
        FileInspectionRepository fileInspection{};
        WorldRepository worldRepository{};
        RuntimeRepository runtimeRepository{};
        StreamingService streamingRepository{};
        EngineRepository engineRepository{};
        EditorRepository editorRepository{};
        // ----------- Repository
    };
}
#endif //APPLICATIONSINGLETONS_H
