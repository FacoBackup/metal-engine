cmake_minimum_required(VERSION 3.30)
project(metal_engine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")

add_executable(
        metal_engine
        src/main.cpp
        src/editor/EditorPanel.cpp
        src/editor/EditorPanel.h
        src/engine/EngineContext.cpp
        src/engine/EngineContext.h
        src/core/vulkan/VulkanUtils.cpp
        src/core/vulkan/VulkanUtils.h
        src/core/vulkan/VulkanContext.cpp
        src/core/vulkan/VulkanContext.h
        src/core/glfw/GLFWContext.cpp
        src/core/glfw/GLFWContext.h
        src/core/gui/GuiContext.cpp
        src/core/gui/GuiContext.h
        src/editor/dock-spaces/viewport/ViewportPanel.cpp
        src/editor/dock-spaces/viewport/ViewportPanel.h
        src/editor/dock-spaces/viewport/EngineFramePanel.cpp
        src/editor/dock-spaces/viewport/EngineFramePanel.h
        src/editor/dock-spaces/docks/AbstractDockPanel.h
        src/ApplicationContext.cpp
        src/ApplicationContext.h
        src/editor/abstract/AbstractPanel.h
        src/editor/abstract/AbstractPanel.cpp
        src/common/ISync.h
        src/engine/service/CameraService.cpp
        src/engine/service/CameraService.h
        src/engine/dto/Camera.h
        src/editor/util/Util.h
        src/common/Inspectable.h
        src/common/Inspectable.cpp
        src/common/InspectedField.h
        src/editor/dock-spaces/inspector/InspectorPanel.cpp
        src/editor/dock-spaces/inspector/InspectorPanel.h
        src/editor/abstract/form/FormPanel.cpp
        src/editor/abstract/form/FormPanel.h
        src/editor/abstract/form/AccordionPanel.cpp
        src/editor/abstract/form/AccordionPanel.h
        src/common/FieldType.h
        src/common/InspectableMember.h
        src/editor/abstract/form/types/BooleanField.cpp
        src/editor/abstract/form/types/BooleanField.h
        src/editor/abstract/form/types/IntField.cpp
        src/editor/abstract/form/types/IntField.h
        src/editor/abstract/form/types/FloatField.cpp
        src/editor/abstract/form/types/FloatField.h
        src/editor/abstract/form/types/MethodField.cpp
        src/editor/abstract/form/types/MethodField.h
        src/editor/util/UIUtil.h
        src/editor/dock-spaces/header/EditorHeaderPanel.cpp
        src/editor/dock-spaces/header/EditorHeaderPanel.h
        src/editor/dock-spaces/header/AsyncTaskPanel.cpp
        src/editor/dock-spaces/header/AsyncTaskPanel.h
        src/editor/dock-spaces/footer/EditorFooterPanel.cpp
        src/editor/dock-spaces/footer/EditorFooterPanel.h
        src/common/Icons.h
        src/editor/service/DockService.cpp
        src/editor/service/DockService.h
        src/editor/dto/DockDTO.h
        src/editor/dto/DockSpace.h
        src/editor/dock-spaces/docks/DockSpacePanel.cpp
        src/editor/dock-spaces/docks/DockSpacePanel.h
        src/editor/dto/DockSpace.cpp
        src/editor/service/ThemeService.cpp
        src/editor/service/ThemeService.h
        src/editor/repository/EditorRepository.h
        src/editor/enum/engine-definitions.h
        src/editor/dock-spaces/files/FilesPanel.cpp
        src/editor/dock-spaces/files/FilesPanel.h
        src/editor/dock-spaces/files/FilesListPanel.cpp
        src/editor/dock-spaces/files/FilesListPanel.h
        src/editor/dock-spaces/files/FilePreviewPanel.cpp
        src/editor/dock-spaces/files/FilePreviewPanel.h
        src/editor/dock-spaces/console/ConsolePanel.cpp
        src/editor/dock-spaces/console/ConsolePanel.h
        src/editor/dock-spaces/metrics/MetricsPanel.cpp
        src/editor/dock-spaces/metrics/MetricsPanel.h
        src/common/LoggerUtil.cpp
        src/common/LoggerUtil.h
        src/editor/dock-spaces/world/WorldPanel.cpp
        src/editor/dock-spaces/world/WorldPanel.h
        src/engine/resource/RuntimeResource.h
        src/engine/service/ShaderService.cpp
        src/engine/service/ShaderService.h
        src/engine/dto/ShaderModule.cpp
        src/engine/dto/ShaderModule.h
        src/engine/passes/AbstractRenderPass.cpp
        src/engine/passes/AbstractRenderPass.h
        src/common/AbstractResourceService.h
        src/engine/resource/FrameBufferAttachment.h
        src/engine/service/FrameBufferService.cpp
        src/engine/service/FrameBufferService.h
        src/engine/resource/FrameBufferInstance.h
        src/engine/service/PipelineService.cpp
        src/engine/service/PipelineService.h
        src/engine/resource/PipelineInstance.h
        src/engine/service/BufferService.cpp
        src/engine/service/BufferService.h
        src/engine/service/DescriptorSetService.cpp
        src/engine/service/DescriptorSetService.h
        src/engine/dto/DescriptorInstance.h
        src/engine/dto/GlobalDataUBO.h
        src/engine/repository/RuntimeRepository.h
        src/engine/resource/BufferInstance.cpp
        src/engine/resource/BufferInstance.h
        src/engine/service/MeshService.cpp
        src/engine/service/MeshService.h
        src/engine/dto/MeshData.h
        src/engine/resource/MeshInstance.h
        src/common/FilesUtil.h
        src/editor/dto/FSEntry.h
        src/editor/enum/EntryType.h
        src/editor/dock-spaces/files/FilesContext.h
        src/common/FileDialogUtil.h
        src/editor/dock-spaces/files/FilesHeaderPanel.cpp
        src/editor/dock-spaces/files/FilesHeaderPanel.h
        src/editor/service/MaterialImporterService.cpp
        src/editor/service/MaterialImporterService.h
        src/editor/service/MeshImporterService.cpp
        src/editor/service/MeshImporterService.h
        src/editor/service/SceneImporterService.cpp
        src/editor/service/SceneImporterService.h
        src/common/AbstractImporter.h
        src/editor/service/TextureImporterService.cpp
        src/editor/service/TextureImporterService.h
        src/editor/dto/EntryMetadata.h
        src/engine/dto/TextureData.h
        src/engine/service/TextureService.cpp
        src/engine/service/TextureService.h
        src/editor/service/FilesService.cpp
        src/editor/service/FilesService.h
        src/editor/service/FileImporterService.cpp
        src/editor/service/FileImporterService.h
        src/editor/dock-spaces/files/FilesContext.cpp
        src/engine/dto/VertexData.h
        src/editor/dock-spaces/viewport/CameraPositionPanel.cpp
        src/editor/dock-spaces/viewport/CameraPositionPanel.h
        src/editor/passes/GridPass.cpp
        src/editor/passes/GridPass.h
        src/engine/passes/CommandBufferRecorder.cpp
        src/engine/passes/CommandBufferRecorder.h
        src/engine/dto/Camera.cpp
        src/engine/repository/WorldRepository.cpp
        src/engine/repository/WorldRepository.h
        src/engine/dto/MetadataComponent.h
        src/engine/enum/ComponentType.cpp
        src/engine/enum/ComponentType.h
        src/engine/dto/AbstractComponent.h
        src/engine/dto/PrimitiveComponent.h
        src/engine/dto/TransformComponent.h
        src/editor/enum/ShadingMode.h
        src/editor/dock-spaces/world/WorldHeaderPanel.cpp
        src/editor/dock-spaces/world/WorldHeaderPanel.h
        src/editor/service/SelectionService.cpp
        src/editor/service/SelectionService.h
        src/engine/dto/MetadataComponent.cpp
        src/engine/dto/TransformComponent.cpp
        src/engine/dto/PrimitiveComponent.cpp
        src/editor/abstract/form/types/ResourceField.cpp
        src/editor/abstract/form/types/ResourceField.h
        src/editor/abstract/form/types/ColorField.cpp
        src/editor/abstract/form/types/ColorField.h
        src/editor/abstract/form/types/Vec4Field.cpp
        src/editor/abstract/form/types/Vec4Field.h
        src/editor/abstract/form/types/Vec3Field.cpp
        src/editor/abstract/form/types/Vec3Field.h
        src/editor/abstract/form/types/Vec2Field.cpp
        src/editor/abstract/form/types/Vec2Field.h
        src/editor/abstract/form/types/StringField.cpp
        src/editor/abstract/form/types/StringField.h
        src/editor/abstract/form/types/ResourceFilesPanel.h
        src/editor/abstract/form/types/ResourceFilesPanel.cpp
        src/engine/service/StreamingService.cpp
        src/engine/service/StreamingService.h
        src/engine/enum/ResourceType.h
        src/editor/dock-spaces/viewport/GizmoPanel.cpp
        src/editor/dock-spaces/viewport/GizmoPanel.h
        src/editor/dock-spaces/viewport/ViewportHeaderPanel.cpp
        src/editor/dock-spaces/viewport/ViewportHeaderPanel.h
        src/editor/dock-spaces/viewport/GizmoSettingsPanel.cpp
        src/editor/dock-spaces/viewport/GizmoSettingsPanel.h
        src/editor/abstract/form/types/QuatField.cpp
        src/editor/abstract/form/types/QuatField.h
        src/engine/service/TransformService.cpp
        src/engine/service/TransformService.h
        src/engine/dto/BoundingBox.h
        src/editor/dto/SparseVoxelOctreeBuilder.cpp
        src/editor/dto/SparseVoxelOctreeBuilder.h
        src/editor/dto/OctreeNode.cpp
        src/editor/dto/OctreeNode.h
        src/engine/dto/VoxelData.h
        src/editor/dto/SceneData.h
        src/engine/passes/impl/PostProcessingPass.cpp
        src/engine/passes/impl/PostProcessingPass.h
        src/engine/passes/impl/HWRayTracingPass.cpp
        src/engine/passes/impl/HWRayTracingPass.h
        src/engine/service/RayTracingService.cpp
        src/engine/service/RayTracingService.h
        src/engine/dto/PipelineBuilder.h
        src/engine/resource/FrameBufferAttachment.cpp
        src/engine/repository/EngineRepository.cpp
        src/engine/repository/EngineRepository.h
        src/common/serialization-definitions.h
        src/editor/service/NotificationService.cpp
        src/editor/service/NotificationService.h
        src/editor/service/AsyncTaskService.cpp
        src/editor/service/AsyncTaskService.h
        src/editor/dto/Notification.h
        src/editor/enum/NotificationSeverity.h
        src/engine/dto/SparseVoxelOctreeData.h
        src/editor/passes/SelectionOutlinePass.cpp
        src/editor/passes/SelectionOutlinePass.h
        src/editor/service/PickingService.cpp
        src/editor/service/PickingService.h
        src/engine/resource/SVOInstance.h
        src/engine/dto/LightData.h
        src/editor/passes/SelectionIDPass.cpp
        src/editor/passes/SelectionIDPass.h
        src/editor/repository/EditorRepository.cpp
        src/editor/dto/GridPushConstant.h
        src/engine/passes/AbstractComputePass.cpp
        src/engine/passes/AbstractComputePass.h
        src/engine/passes/AbstractPass.cpp
        src/engine/passes/AbstractPass.h
        src/engine/util/ImageUtils.h
        src/engine/service/MaterialService.cpp
        src/engine/service/MaterialService.h
        src/engine/service/LightService.cpp
        src/engine/service/LightService.h
        src/engine/dto/VolumeComponent.cpp
        src/engine/dto/VolumeComponent.h
        src/engine/dto/DescriptorBinding.h
        src/editor/dto/AsyncTask.h
        src/editor/service/VoxelImporterService.cpp
        src/editor/service/VoxelImporterService.h
        src/engine/dto/VolumeData.h
        src/engine/service/VoxelService.cpp
        src/engine/service/VoxelService.h
        src/common/ISerialize.cpp
        src/editor/abstract/form/ChildPanel.cpp
        src/editor/abstract/form/ChildPanel.h
        src/editor/dto/SceneImportSettingsDTO.h
        src/editor/panel/NotificationsPanel.cpp
        src/editor/panel/NotificationsPanel.h
        src/editor/panel/FileImportModalPanel.cpp
        src/editor/panel/FileImportModalPanel.h
        src/engine/frame-builder/EngineFrameBuilder.cpp
        src/engine/frame-builder/EngineFrameBuilder.h
        src/engine/frame-builder/EngineFrame.cpp
        src/engine/frame-builder/EngineFrame.h
        src/engine/frame-builder/structures/FramebufferBuilder.cpp
        src/engine/frame-builder/structures/FramebufferBuilder.h
        src/engine/frame-builder/structures/TextureBuilder.cpp
        src/engine/frame-builder/structures/TextureBuilder.h
        src/engine/frame-builder/structures/BufferBuilder.cpp
        src/engine/frame-builder/structures/BufferBuilder.h
        src/engine/frame-builder/structures/ResourceBuilder.h
        src/engine/dto/PipelineBuilder.cpp
        src/engine/service/DescriptorSetService.cpp
        src/engine/frame-builder/structures/CommandBufferRecorderBuilder.cpp
        src/engine/passes/impl/TemporalAccumulationPass.cpp
        src/engine/passes/impl/TemporalAccumulationPass.h
        src/engine/passes/impl/SpatialFilterPass.cpp
        src/engine/passes/impl/SpatialFilterPass.h
        src/engine/enum/ComponentType.cpp
        src/editor/dto/SceneEntityData.h
        src/editor/dock-spaces/repositories/RepositoriesPanel.cpp
        src/editor/dock-spaces/repositories/RepositoriesPanel.h
        src/engine/service/VolumeService.cpp
        src/engine/service/VolumeService.h
        src/common/IContextMember.h
        src/common/IService.h
        src/common/IRepository.h
        src/common/IContextMember.cpp
        src/core/DirectoryService.cpp
        src/core/DirectoryService.h
        src/common/IDisposable.h
        src/core/FrameService.cpp
        src/core/FrameService.h
)


# ------ LIBS
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Vulkan)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(glslang CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)
find_package(nfd CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(meshoptimizer CONFIG REQUIRED)
find_package(cereal CONFIG REQUIRED)
find_package(OpenVDB CONFIG REQUIRED)

set(COMMON_LIBRARIES
        glfw
        nfd::nfd
        cereal::cereal
        meshoptimizer::meshoptimizer
        assimp::assimp
        Vulkan::Headers
        vk-bootstrap
        vk-bootstrap-compiler-warnings
        GPUOpen::VulkanMemoryAllocator
        glm::glm
        imgui::imgui
        imguizmo::imguizmo
        spdlog::spdlog
        nlohmann_json::nlohmann_json

        glslang::glslang
        glslang::glslang-default-resource-limits
        glslang::SPVRemapper
        OpenVDB::openvdb
)
target_link_libraries(metal_engine PRIVATE ${COMMON_LIBRARIES})
add_subdirectory(dependencies/vk-bootstrap)
# ------ LIBS
# ------ STATIC RESOURCES
file(COPY "${CMAKE_SOURCE_DIR}/resources" DESTINATION "${CMAKE_BINARY_DIR}/")
# ------ STATIC RESOURCES

# ------ VCPKG

#set(CMAKE_TOOLCHAIN_FILE "Users/cafe/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(VCPKG_JSON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg.json")

# Include vcpkg toolchain
if (EXISTS "${VCPKG_JSON_PATH}")
    set(VCPKG_TOOLCHAIN "${CMAKE_TOOLCHAIN_FILE}")
    if (EXISTS "${VCPKG_TOOLCHAIN}")
        message(STATUS "Using Vcpkg toolchain file: ${VCPKG_TOOLCHAIN}")
        include("${VCPKG_TOOLCHAIN}")
    else ()
        message(FATAL_ERROR "Vcpkg toolchain file not found. Please install Vcpkg and create a vcpkg.json file for your project.")
    endif ()
else ()
    message(FATAL_ERROR "Vcpkg JSON file not found. Please create a vcpkg.json file for your project.")
endif ()
# ------ VCPKG