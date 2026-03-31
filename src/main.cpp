#include <boost-di.h>
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
#include "editor/service/EditorCameraService.h"
#include "editor/service/ThemeService.h"
#include "editor/repository/EditorRepository.h"
#include "editor/repository/DockRepository.h"
#include "editor/repository/EditorCameraRepository.h"
#include "editor/repository/AIAssistantRepository.h"

#include "engine/repository/EngineRepository.h"
#include "engine/repository/RuntimeRepository.h"
#include "engine/repository/WorldRepository.h"

#include "engine/service/BufferService.h"
#include "engine/service/CameraService.h"
#include "engine/service/CommandBufferRecorderService.h"
#include "engine/service/DescriptorSetService.h"
#include "engine/service/RenderTargetService.h"
#include "engine/service/LightService.h"
#include "engine/service/PhysicsService.h"
#include "engine/service/LuaService.h"
#include "engine/service/LevelService.h"
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
#include "editor/ui/viewport/GizmoTransformStrategy.h"
#include "engine/repository/CameraRepository.h"
#include "engine/service/DirtyStateService.h"

int main(int, char **) {
    namespace di = boost::ext::di;
    auto injector = di::make_injector(
        di::bind<Metal::WindowService>().in(di::singleton),
        di::bind<Metal::VulkanContext>().in(di::singleton),
        di::bind<Metal::ImGuiService>().in(di::singleton),
        di::bind<Metal::DirectoryService>().in(di::singleton),
        di::bind<Metal::SnapshotService>().in(di::singleton),
        di::bind<Metal::ThreadManager>().in(di::singleton),
        di::bind<Metal::AsyncSyncService>().in(di::singleton),
        di::bind<Metal::DirtyStateService>().in(di::singleton),
        di::bind<Metal::FilesService>().in(di::singleton),
        di::bind<Metal::EngineContext>().in(di::singleton),
        di::bind<Metal::EngineRepository>().in(di::singleton),
        di::bind<Metal::EditorRepository>().in(di::singleton),
        di::bind<Metal::EditorCameraRepository>().in(di::singleton),
        di::bind<Metal::DockRepository>().in(di::singleton),
        di::bind<Metal::AIAssistantRepository>().in(di::singleton),
        di::bind<Metal::RuntimeRepository>().in(di::singleton),
        di::bind<Metal::WorldRepository>().in(di::singleton),
        di::bind<Metal::MeshService>().in(di::singleton),
        di::bind<Metal::TextureService>().in(di::singleton),
        di::bind<Metal::RenderTargetService>().in(di::singleton),
        di::bind<Metal::PipelineService>().in(di::singleton),
        di::bind<Metal::BufferService>().in(di::singleton),
        di::bind<Metal::CommandBufferRecorderService>().in(di::singleton),
        di::bind<Metal::NotificationService>().in(di::singleton),
        di::bind<Metal::HistoryService>().in(di::singleton),
        di::bind<Metal::GizmoTransformStrategy>().in(di::singleton),
        di::bind<Metal::HttpService>().in(di::singleton),
        di::bind<Metal::DescriptorSetService>().in(di::singleton),
        di::bind<Metal::ThemeService>().in(di::singleton),
        di::bind<Metal::DockService>().in(di::singleton),
        di::bind<Metal::MaterialService>().in(di::singleton),
        di::bind<Metal::AsyncTaskService>().in(di::singleton),
        di::bind<Metal::AIAssistantService>().in(di::singleton),
        di::bind<Metal::WorldToolProvider>().in(di::singleton),
        di::bind<Metal::ScriptingToolProvider>().in(di::singleton),
        di::bind<Metal::LayoutingToolProvider>().in(di::singleton),
        di::bind<Metal::InspectionToolProvider>().in(di::singleton),
        di::bind<Metal::SceneImporterService>().in(di::singleton),
        di::bind<Metal::SelectionService>().in(di::singleton),
        di::bind<Metal::MeshImporterService>().in(di::singleton),
        di::bind<Metal::MaterialImporterService>().in(di::singleton),
        di::bind<Metal::TextureImporterService>().in(di::singleton),
        di::bind<Metal::FileImporterService>().in(di::singleton),
        di::bind<Metal::EditorCameraService>().in(di::singleton),
        di::bind<Metal::CameraService>().in(di::singleton),
        di::bind<Metal::CameraRepository>().in(di::singleton),
        di::bind<Metal::TransformService>().in(di::singleton),
        di::bind<Metal::PickingService>().in(di::singleton),
        di::bind<Metal::PhysicsService>().in(di::singleton),
        di::bind<Metal::LuaService>().in(di::singleton),
        di::bind<Metal::LevelService>().in(di::singleton),
        di::bind<Metal::LightService>().in(di::singleton),
        di::bind<Metal::StreamingService>().in(di::singleton),
        di::bind<Metal::BLASService>().in(di::singleton),
        di::bind<Metal::TLASService>().in(di::singleton),
        di::bind<Metal::ShaderService>().in(di::singleton),
        di::bind<Metal::FrameService>().in(di::singleton)
    );

    auto &frameService = injector.create<Metal::FrameService &>();
    auto &directoryService = injector.create<Metal::DirectoryService &>();

    Metal::AbstractPanel::serviceProvider = [&](const std::string &name) -> void * {
        if (name == "WindowService") return &injector.create<Metal::WindowService &>();
        if (name == "VulkanContext") return &injector.create<Metal::VulkanContext &>();
        if (name == "ImGuiService") return &injector.create<Metal::ImGuiService &>();
        if (name == "DirectoryService") return &injector.create<Metal::DirectoryService &>();
        if (name == "SnapshotService") return &injector.create<Metal::SnapshotService &>();
        if (name == "ThreadManager") return &injector.create<Metal::ThreadManager &>();
        if (name == "AsyncSyncService") return &injector.create<Metal::AsyncSyncService &>();
        if (name == "DirtyStateService") return &injector.create<Metal::DirtyStateService &>();
        if (name == "FilesService") return &injector.create<Metal::FilesService &>();
        if (name == "EngineContext") return &injector.create<Metal::EngineContext &>();
        if (name == "EngineRepository") return &injector.create<Metal::EngineRepository &>();
        if (name == "EditorRepository") return &injector.create<Metal::EditorRepository &>();
        if (name == "EditorCameraRepository") return &injector.create<Metal::EditorCameraRepository &>();
        if (name == "DockRepository") return &injector.create<Metal::DockRepository &>();
        if (name == "AIAssistantRepository") return &injector.create<Metal::AIAssistantRepository &>();
        if (name == "RuntimeRepository") return &injector.create<Metal::RuntimeRepository &>();
        if (name == "WorldRepository") return &injector.create<Metal::WorldRepository &>();
        if (name == "MeshService") return &injector.create<Metal::MeshService &>();
        if (name == "TextureService") return &injector.create<Metal::TextureService &>();
        if (name == "RenderTargetService") return &injector.create<Metal::RenderTargetService &>();
        if (name == "PipelineService") return &injector.create<Metal::PipelineService &>();
        if (name == "BufferService") return &injector.create<Metal::BufferService &>();
        if (name == "CommandBufferRecorderService") return &injector.create<Metal::CommandBufferRecorderService &>();
        if (name == "NotificationService") return &injector.create<Metal::NotificationService &>();
        if (name == "HistoryService") return &injector.create<Metal::HistoryService &>();
        if (name == "GizmoTransformStrategy") return &injector.create<Metal::GizmoTransformStrategy &>();
        if (name == "HttpService") return &injector.create<Metal::HttpService &>();
        if (name == "DescriptorSetService") return &injector.create<Metal::DescriptorSetService &>();
        if (name == "ThemeService") return &injector.create<Metal::ThemeService &>();
        if (name == "DockService") return &injector.create<Metal::DockService &>();
        if (name == "MaterialService") return &injector.create<Metal::MaterialService &>();
        if (name == "AsyncTaskService") return &injector.create<Metal::AsyncTaskService &>();
        if (name == "AIAssistantService") return &injector.create<Metal::AIAssistantService &>();
        if (name == "WorldToolProvider") return &injector.create<Metal::WorldToolProvider &>();
        if (name == "ScriptingToolProvider") return &injector.create<Metal::ScriptingToolProvider &>();
        if (name == "LayoutingToolProvider") return &injector.create<Metal::LayoutingToolProvider &>();
        if (name == "InspectionToolProvider") return &injector.create<Metal::InspectionToolProvider &>();
        if (name == "SceneImporterService") return &injector.create<Metal::SceneImporterService &>();
        if (name == "SelectionService") return &injector.create<Metal::SelectionService &>();
        if (name == "MeshImporterService") return &injector.create<Metal::MeshImporterService &>();
        if (name == "MaterialImporterService") return &injector.create<Metal::MaterialImporterService &>();
        if (name == "TextureImporterService") return &injector.create<Metal::TextureImporterService &>();
        if (name == "FileImporterService") return &injector.create<Metal::FileImporterService &>();
        if (name == "EditorCameraService") return &injector.create<Metal::EditorCameraService &>();
        if (name == "CameraService") return &injector.create<Metal::CameraService &>();
        if (name == "CameraRepository") return &injector.create<Metal::CameraRepository &>();
        if (name == "TransformService") return &injector.create<Metal::TransformService &>();
        if (name == "PickingService") return &injector.create<Metal::PickingService &>();
        if (name == "PhysicsService") return &injector.create<Metal::PhysicsService &>();
        if (name == "LuaService") return &injector.create<Metal::LuaService &>();
        if (name == "LevelService") return &injector.create<Metal::LevelService &>();
        if (name == "LightService") return &injector.create<Metal::LightService &>();
        if (name == "StreamingService") return &injector.create<Metal::StreamingService &>();
        if (name == "BLASService") return &injector.create<Metal::BLASService &>();
        if (name == "TLASService") return &injector.create<Metal::TLASService &>();
        if (name == "ShaderService") return &injector.create<Metal::ShaderService &>();
        if (name == "FrameService") return &injector.create<Metal::FrameService &>();
        return nullptr;
    };

    directoryService.updateRootPath(false);
    Metal::EditorPanel editor{};
    editor.performInjection();
    editor.onInitialize();

    frameService.setPanel(&editor);
    if (!frameService.isValidContext()) {
        printf("GLFW: Vulkan Not Supported\n");
        return 1;
    }
    frameService.start();
    injector.create<Metal::ImGuiService &>().disposeManually();
    injector.create<Metal::VulkanContext &>().disposeManually();
    injector.create<Metal::WindowService &>().disposeManually();

    return 0;
}
