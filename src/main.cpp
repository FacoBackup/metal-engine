#include "ApplicationContext.h"
#include "core/DirectoryService.h"
#include "core/service/SnapshotService.h"
#include "core/FrameService.h"
#include "core/WindowService.h"
#include "core/ImGuiService.h"
#include "core/VulkanContext.h"
#include "core/service/ThreadManager.h"
#include "engine/service/AsyncSyncService.h"
#include "editor/ui/EditorPanel.h"
#include "editor/service/AsyncTaskService.h"
#include "editor/service/DockService.h"
#include "editor/service/FileImporterService.h"
#include "editor/service/FilesService.h"
#include "editor/service/HistoryService.h"
#include "editor/service/HttpService.h"
#include "editor/service/MaterialImporterService.h"
#include "editor/service/AIAssistantService.h"
#include "editor/tools/WorldToolProvider.h"
#include "editor/tools/ScriptingToolProvider.h"
#include "editor/tools/LayoutingToolProvider.h"
#include "editor/tools/InspectionToolProvider.h"
#include "editor/service/MeshImporterService.h"
#include "editor/service/NotificationService.h"
#include "editor/service/PickingService.h"
#include "editor/service/SceneImporterService.h"
#include "editor/service/SelectionService.h"
#include "editor/service/TextureImporterService.h"
#include "editor/service/ThemeService.h"
#include "editor/repository/EditorRepository.h"
#include "editor/repository/DockRepository.h"
#include "editor/repository/AIAssistantRepository.h"

#include "engine/repository/EngineRepository.h"
#include "engine/repository/RuntimeRepository.h"
#include "engine/repository/WorldRepository.h"

#include "engine/service/BufferService.h"
#include "engine/service/CameraService.h"
#include "engine/service/CommandBufferRecorderService.h"
#include "engine/service/DescriptorSetService.h"
#include "engine/service/FrameBufferService.h"
#include "engine/service/LightService.h"
#include "engine/service/PhysicsService.h"
#include "engine/service/LuaService.h"
#include "engine/service/MaterialService.h"
#include "engine/service/MeshService.h"
#include "engine/service/PipelineService.h"
#include "engine/service/BLASService.h"
#include "engine/service/TLASService.h"
#include "engine/service/ShaderService.h"
#include "engine/service/StreamingService.h"
#include "engine/service/TextureService.h"
#include "engine/service/TransformService.h"
#include "engine/EngineContext.h"
#include "engine/service/DirtyStateService.h"

int main(int, char **) {
    std::shared_ptr<Metal::ApplicationContext> context = std::make_shared<Metal::ApplicationContext>(true);

    // --- ORDER MATTERS
    context->registerSingleton(context);
    context->registerSingleton(std::make_shared<Metal::WindowService>());
    context->registerSingleton(std::make_shared<Metal::VulkanContext>());
    context->registerSingleton(std::make_shared<Metal::ImGuiService>());
    context->registerSingleton(std::make_shared<Metal::DirectoryService>());
    context->registerSingleton(std::make_shared<Metal::SnapshotService>());
    context->registerSingleton(std::make_shared<Metal::ThreadManager>());
    context->registerSingleton(std::make_shared<Metal::AsyncSyncService>());
    // --- ORDER MATTERS

    context->registerSingleton(std::make_shared<Metal::DirtyStateService>());
    context->registerSingleton(std::make_shared<Metal::FilesService>());
    context->registerSingleton(std::make_shared<Metal::EngineContext>());
    context->registerSingleton(std::make_shared<Metal::EngineRepository>());
    context->registerSingleton(std::make_shared<Metal::EditorRepository>());
    context->registerSingleton(std::make_shared<Metal::DockRepository>());
    context->registerSingleton(std::make_shared<Metal::AIAssistantRepository>());
    context->registerSingleton(std::make_shared<Metal::RuntimeRepository>());
    context->registerSingleton(std::make_shared<Metal::WorldRepository>());
    context->registerSingleton(std::make_shared<Metal::MeshService>());
    context->registerSingleton(std::make_shared<Metal::TextureService>());
    context->registerSingleton(std::make_shared<Metal::FrameBufferService>());
    context->registerSingleton(std::make_shared<Metal::PipelineService>());
    context->registerSingleton(std::make_shared<Metal::BufferService>());
    context->registerSingleton(std::make_shared<Metal::CommandBufferRecorderService>());
    context->registerSingleton(std::make_shared<Metal::NotificationService>());
    context->registerSingleton(std::make_shared<Metal::HistoryService>());
    context->registerSingleton(std::make_shared<Metal::HttpService>());
    context->registerSingleton(std::make_shared<Metal::DescriptorSetService>());
    context->registerSingleton(std::make_shared<Metal::ThemeService>());
    context->registerSingleton(std::make_shared<Metal::DockService>());
    context->registerSingleton(std::make_shared<Metal::MaterialService>());
    context->registerSingleton(std::make_shared<Metal::AsyncTaskService>());
    context->registerSingleton(std::make_shared<Metal::AIAssistantService>());
    context->registerSingleton(std::make_shared<Metal::WorldToolProvider>());
    context->registerSingleton(std::make_shared<Metal::ScriptingToolProvider>());
    context->registerSingleton(std::make_shared<Metal::LayoutingToolProvider>());
    context->registerSingleton(std::make_shared<Metal::InspectionToolProvider>());
    context->registerSingleton(std::make_shared<Metal::SceneImporterService>());

    context->registerSingleton(std::make_shared<Metal::SelectionService>());
    context->registerSingleton(std::make_shared<Metal::MeshImporterService>());
    context->registerSingleton(std::make_shared<Metal::MaterialImporterService>());
    context->registerSingleton(std::make_shared<Metal::TextureImporterService>());
    context->registerSingleton(std::make_shared<Metal::FileImporterService>());
    context->registerSingleton(std::make_shared<Metal::CameraService>());
    context->registerSingleton(std::make_shared<Metal::TransformService>());
    context->registerSingleton(std::make_shared<Metal::PickingService>());
    context->registerSingleton(std::make_shared<Metal::PhysicsService>());
    context->registerSingleton(std::make_shared<Metal::LuaService>());
    context->registerSingleton(std::make_shared<Metal::LightService>());
    context->registerSingleton(std::make_shared<Metal::StreamingService>());

    context->registerSingleton(std::make_shared<Metal::BLASService>());
    context->registerSingleton(std::make_shared<Metal::TLASService>());
    context->registerSingleton(std::make_shared<Metal::ShaderService>());
    context->registerSingleton(std::make_shared<Metal::FrameService>());

    context->onInitialize();
    auto &frameService = context->getSingleton<Metal::FrameService>();
    auto &directoryService = context->getSingleton<Metal::DirectoryService>();
    directoryService.updateRootPath(false);
    Metal::EditorPanel editor{};
    context->injectDependencies(&editor);
    editor.onInitialize();

    frameService.setPanel(&editor);
    if (!frameService.isValidContext()) {
        printf("GLFW: Vulkan Not Supported\n");
        return 1;
    }
    frameService.start();
    context->dispose();
    context->getSingleton<Metal::ImGuiService>().disposeManually();
    context->getSingleton<Metal::VulkanContext>().disposeManually();
    context->getSingleton<Metal::WindowService>().disposeManually();

    return 0;
}
