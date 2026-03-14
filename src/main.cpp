#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define VMA_IMPLEMENTATION
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "ApplicationContext.h"
#include "core/DirectoryService.h"
#include "core/FrameService.h"
#include "core/glfw/GLFWContext.h"
#include "core/gui/GuiContext.h"
#include "core/vulkan/VulkanContext.h"
#include "editor/EditorPanel.h"
#include "editor/service/AsyncTaskService.h"
#include "editor/service/DockService.h"
#include "editor/service/FileImporterService.h"
#include "editor/service/FilesService.h"
#include "editor/service/MaterialImporterService.h"
#include "editor/service/MeshImporterService.h"
#include "editor/service/NotificationService.h"
#include "editor/service/PickingService.h"
#include "editor/service/SceneImporterService.h"
#include "editor/service/SelectionService.h"
#include "editor/service/TextureImporterService.h"
#include "editor/service/ThemeService.h"
#include "editor/service/VoxelImporterService.h"
#include "editor/repository/EditorRepository.h"

#include "engine/repository/EngineRepository.h"
#include "engine/repository/RuntimeRepository.h"
#include "engine/repository/WorldRepository.h"

#include "engine/service/BufferService.h"
#include "engine/service/CameraService.h"
#include "engine/service/CommandBufferRecorderService.h"
#include "engine/service/DescriptorSetService.h"
#include "engine/service/FrameBufferService.h"
#include "engine/service/LightService.h"
#include "engine/service/MaterialService.h"
#include "engine/service/MeshService.h"
#include "engine/service/PipelineService.h"
#include "engine/service/RayTracingService.h"
#include "engine/service/ShaderService.h"
#include "engine/service/StreamingService.h"
#include "engine/service/TextureService.h"
#include "engine/service/TransformService.h"
#include "engine/service/VolumeService.h"
#include "engine/service/VoxelService.h"
#include "engine/EngineContext.h"

int main(int, char **) {
    Metal::ApplicationContext context{true};

    // --- ORDER MATTERS
    context.registerSingleton(std::make_unique<Metal::GLFWContext>());
    context.registerSingleton(std::make_unique<Metal::VulkanContext>());
    context.registerSingleton(std::make_unique<Metal::GuiContext>());
    context.registerSingleton(std::make_unique<Metal::DirectoryService>());
    // --- ORDER MATTERS

    context.registerSingleton(std::make_unique<Metal::FilesService>());
    context.registerSingleton(std::make_unique<Metal::EngineContext>());
    context.registerSingleton(std::make_unique<Metal::EngineRepository>());
    context.registerSingleton(std::make_unique<Metal::EditorRepository>());
    context.registerSingleton(std::make_unique<Metal::RuntimeRepository>());
    context.registerSingleton(std::make_unique<Metal::WorldRepository>());
    context.registerSingleton(std::make_unique<Metal::MeshService>());
    context.registerSingleton(std::make_unique<Metal::TextureService>());
    context.registerSingleton(std::make_unique<Metal::FrameBufferService>());
    context.registerSingleton(std::make_unique<Metal::PipelineService>());
    context.registerSingleton(std::make_unique<Metal::BufferService>());
    context.registerSingleton(std::make_unique<Metal::CommandBufferRecorderService>());
    context.registerSingleton(std::make_unique<Metal::NotificationService>());
    context.registerSingleton(std::make_unique<Metal::DescriptorSetService>());
    context.registerSingleton(std::make_unique<Metal::ThemeService>());
    context.registerSingleton(std::make_unique<Metal::DockService>());
    context.registerSingleton(std::make_unique<Metal::MaterialService>());
    context.registerSingleton(std::make_unique<Metal::AsyncTaskService>());
    context.registerSingleton(std::make_unique<Metal::SceneImporterService>());

    context.registerSingleton(std::make_unique<Metal::SelectionService>());
    context.registerSingleton(std::make_unique<Metal::MeshImporterService>());
    context.registerSingleton(std::make_unique<Metal::MaterialImporterService>());
    context.registerSingleton(std::make_unique<Metal::TextureImporterService>());
    context.registerSingleton(std::make_unique<Metal::FileImporterService>());
    context.registerSingleton(std::make_unique<Metal::CameraService>());
    context.registerSingleton(std::make_unique<Metal::TransformService>());
    context.registerSingleton(std::make_unique<Metal::PickingService>());
    context.registerSingleton(std::make_unique<Metal::VolumeService>());
    context.registerSingleton(std::make_unique<Metal::VoxelImporterService>());
    context.registerSingleton(std::make_unique<Metal::LightService>());
    context.registerSingleton(std::make_unique<Metal::StreamingService>());

    context.registerSingleton(std::make_unique<Metal::VoxelService>());
    context.registerSingleton(std::make_unique<Metal::RayTracingService>());
    context.registerSingleton(std::make_unique<Metal::ShaderService>());
    context.registerSingleton(std::make_unique<Metal::EditorPanel>());
    context.registerSingleton(std::make_unique<Metal::FrameService>());

    context.onInitialize();
    auto &frameService = context.getSingleton<Metal::FrameService>();
    auto &directoryService = context.getSingleton<Metal::DirectoryService>();
    directoryService.updateRootPath(false);
    auto &editor = context.getSingleton<Metal::EditorPanel>();

    frameService.setPanel(&editor);
    if (!frameService.isValidContext()) {
        printf("GLFW: Vulkan Not Supported\n");
        return 1;
    }
    frameService.onInitialize();
    context.dispose();
    return 0;
}
