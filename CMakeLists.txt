cmake_minimum_required(VERSION 3.30)
project(metal_engine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")

add_executable(
        metal_engine
        src/main.cpp
        src/context/editor/EditorPanel.cpp
        src/context/editor/EditorPanel.h
        src/context/editor/abstract/IPanel.h
        src/context/engine/EngineContext.cpp
        src/context/engine/EngineContext.h
        src/util/VulkanUtils.cpp
        src/util/VulkanUtils.h
        src/context/vulkan/VulkanContext.cpp
        src/context/vulkan/VulkanContext.h
        src/context/glfw/GLFWContext.cpp
        src/context/glfw/GLFWContext.h
        src/context/gui/GuiContext.cpp
        src/context/gui/GuiContext.h
        src/context/editor/dock-spaces/viewport/ViewportPanel.cpp
        src/context/editor/dock-spaces/viewport/ViewportPanel.h
        src/context/editor/dock-spaces/docks/AbstractDockPanel.h
        src/context/ApplicationContext.cpp
        src/context/ApplicationContext.h
        src/context/editor/abstract/AbstractPanel.h
        src/context/editor/abstract/AbstractPanel.cpp
        src/common/interface/Synchornizable.h
        src/service/camera/CameraService.cpp
        src/service/camera/CameraService.h
        src/service/camera/Camera.h
        src/util/Util.h
        src/common/inspection/Inspectable.h
        src/common/inspection/Inspectable.cpp
        src/common/inspection/InspectedField.h
        src/context/editor/dock-spaces/inspector/InspectorPanel.cpp
        src/context/editor/dock-spaces/inspector/InspectorPanel.h
        src/context/editor/abstract/form/FormPanel.cpp
        src/context/editor/abstract/form/FormPanel.h
        src/context/editor/abstract/form/AccordionPanel.cpp
        src/context/editor/abstract/form/AccordionPanel.h
        src/enum/FieldType.h
        src/common/inspection/InspectableMember.h
        src/common/AbstractRuntimeComponent.h
        src/context/editor/abstract/form/types/BooleanField.cpp
        src/context/editor/abstract/form/types/BooleanField.h
        src/context/editor/abstract/form/types/IntField.cpp
        src/context/editor/abstract/form/types/IntField.h
        src/context/editor/abstract/form/types/FloatField.cpp
        src/context/editor/abstract/form/types/FloatField.h
        src/context/editor/abstract/form/types/MethodField.cpp
        src/context/editor/abstract/form/types/MethodField.h
        src/util/UIUtil.h
        src/context/editor/dock-spaces/header/EditorHeaderPanel.cpp
        src/context/editor/dock-spaces/header/EditorHeaderPanel.h
        src/context/editor/dock-spaces/header/GlobalSettingsPanel.cpp
        src/context/editor/dock-spaces/header/GlobalSettingsPanel.h
        src/common/interface/Icons.h
        src/service/dock/DockService.cpp
        src/service/dock/DockService.h
        src/repository/dock/DockRepository.cpp
        src/repository/dock/DockRepository.h
        src/repository/dock/DockDTO.h
        src/repository/dock/DockSpace.h
        src/repository/dock/DockPosition.h
        src/context/editor/dock-spaces/docks/DockSpacePanel.cpp
        src/context/editor/dock-spaces/docks/DockSpacePanel.h
        src/repository/dock/DockSpace.cpp
        src/service/theme/ThemeService.cpp
        src/service/theme/ThemeService.h
        src/repository/editor/EditorRepository.h
        src/enum/engine-definitions.h
        src/context/editor/dock-spaces/metrics/MetricsPanel.cpp
        src/context/editor/dock-spaces/metrics/MetricsPanel.h
        src/context/editor/dock-spaces/files/FilesPanel.cpp
        src/context/editor/dock-spaces/files/FilesPanel.h
        src/context/editor/dock-spaces/console/ConsolePanel.cpp
        src/context/editor/dock-spaces/console/ConsolePanel.h
        src/context/editor/dock-spaces/hierarchy/HierarchyPanel.cpp
        src/context/editor/dock-spaces/hierarchy/HierarchyPanel.h
        src/repository/abstract/RuntimeResource.h
        src/util/ShaderUtil.cpp
        src/util/ShaderUtil.h
        src/service/pipeline/ShaderModule.cpp
        src/service/pipeline/ShaderModule.h
        src/context/engine/render-pass/AbstractRenderPass.cpp
        src/context/engine/render-pass/AbstractRenderPass.h
        src/context/engine/render-pass/impl/OpaqueRenderPass.cpp
        src/context/engine/render-pass/impl/OpaqueRenderPass.h
        src/service/texture/TextureInstance.h
        src/repository/abstract/ResourceRepository.h
        src/service/abstract/AbstractResourceService.cpp
        src/service/abstract/AbstractResourceService.h
        src/service/framebuffer/FrameBufferAttachment.h
        src/repository/framebuffer/CoreFrameBuffers.cpp
        src/repository/framebuffer/CoreFrameBuffers.h
        src/repository/abstract/AbstractCoreRepository.h
        src/repository/abstract/AbstractCoreRepository.cpp
        src/repository/pipeline/CorePipelines.cpp
        src/repository/pipeline/CorePipelines.h
        src/service/framebuffer/FrameBufferService.cpp
        src/service/framebuffer/FrameBufferService.h
        src/service/framebuffer/FrameBufferInstance.h
        src/service/pipeline/PipelineService.cpp
        src/service/pipeline/PipelineService.h
        src/service/pipeline/PipelineInstance.h
        src/repository/buffers/CoreBuffers.cpp
        src/repository/buffers/CoreBuffers.h
        src/service/buffer/BufferService.cpp
        src/service/buffer/BufferService.h
        src/service/descriptor/DescriptorService.cpp
        src/service/descriptor/DescriptorService.h
        src/service/descriptor/DescriptorInstance.cpp
        src/service/descriptor/DescriptorInstance.h
        src/dto/ubo/GlobalDataUBO.h
        src/repository/runtime/RuntimeRepository.h
        src/repository/descriptors/CoreDescriptorSets.cpp
        src/repository/descriptors/CoreDescriptorSets.h
        src/service/buffer/BufferInstance.cpp
        src/service/buffer/BufferInstance.h
        src/service/pipeline/PipelineInstance.cpp
        src/service/mesh/MeshService.cpp
        src/service/mesh/MeshService.h
        src/service/mesh/MeshData.h
        src/service/mesh/MeshInstance.cpp
        src/service/mesh/MeshInstance.h
        src/util/FilesUtil.h
        src/dto/file/FileEntry.h
        src/enum/EntryType.h
        src/context/editor/dock-spaces/files/FilesContext.h
        src/util/FileDialogUtil.h
        src/context/editor/dock-spaces/files/FilesHeader.cpp
        src/context/editor/dock-spaces/files/FilesHeader.h
        src/service/mesh/SceneImporterService.cpp
        src/service/mesh/SceneImporterService.h
        src/service/abstract/AbstractImporter.h
        src/enum/LevelOfDetail.h
        src/enum/LevelOfDetail.cpp
        src/service/texture/TextureImporterService.cpp
        src/service/texture/TextureImporterService.h
        src/dto/file/FileMetadata.h
        src/service/texture/TextureData.h
        src/service/texture/TextureService.cpp
        src/service/texture/TextureService.h
        src/service/files/FilesService.cpp
        src/service/files/FilesService.h
        src/context/editor/dock-spaces/files/FilesContext.cpp
        src/dto/push-constant/MeshPushConstant.h
        src/service/mesh/VertexData.h
        src/context/editor/dock-spaces/viewport/CameraPositionPanel.cpp
        src/context/editor/dock-spaces/viewport/CameraPositionPanel.h
        src/context/engine/render-pass/impl/tools/GridPass.cpp
        src/context/engine/render-pass/impl/tools/GridPass.h
        src/context/engine/passes/CommandBufferRecorder.cpp
        src/context/engine/passes/CommandBufferRecorder.h
        src/repository/render-pass/CoreRenderPasses.cpp
        src/repository/render-pass/CoreRenderPasses.h
        src/service/camera/Camera.cpp
        src/repository/world/WorldRepository.cpp
        src/repository/world/WorldRepository.h
        src/repository/world/impl/Entity.h
        src/enum/ComponentType.h
        src/repository/world/impl/AbstractComponent.h
        src/repository/world/components/MeshComponent.h
        src/repository/world/components/TransformComponent.h
        src/enum/ShadingMode.h
        src/enum/EditorMode.h
        src/context/editor/dock-spaces/hierarchy/HierarchyHeaderPanel.cpp
        src/context/editor/dock-spaces/hierarchy/HierarchyHeaderPanel.h
        src/service/selection/SelectionService.cpp
        src/service/selection/SelectionService.h
        src/repository/world/impl/Entity.cpp
        src/repository/world/components/TransformComponent.cpp
        src/repository/world/components/MeshComponent.cpp
        src/context/editor/abstract/form/types/ResourceField.cpp
        src/context/editor/abstract/form/types/ResourceField.h
        src/context/editor/abstract/form/types/ColorField.cpp
        src/context/editor/abstract/form/types/ColorField.h
        src/context/editor/abstract/form/types/Vec4Field.cpp
        src/context/editor/abstract/form/types/Vec4Field.h
        src/context/editor/abstract/form/types/Vec3Field.cpp
        src/context/editor/abstract/form/types/Vec3Field.h
        src/context/editor/abstract/form/types/Vec2Field.cpp
        src/context/editor/abstract/form/types/Vec2Field.h
        src/context/editor/abstract/form/types/StringField.cpp
        src/context/editor/abstract/form/types/StringField.h
        src/context/editor/abstract/form/types/ResourceFilesPanel.h
        src/context/editor/abstract/form/types/ResourceFilesPanel.cpp
        src/repository/streaming/StreamingRepository.cpp
        src/repository/streaming/StreamingRepository.h
        src/enum/ResourceType.h
        src/context/editor/dock-spaces/viewport/GizmoPanel.cpp
        src/context/editor/dock-spaces/viewport/GizmoPanel.h
        src/context/editor/dock-spaces/viewport/ViewportHeaderPanel.cpp
        src/context/editor/dock-spaces/viewport/ViewportHeaderPanel.h
        src/context/editor/dock-spaces/viewport/GizmoSettingsPanel.cpp
        src/context/editor/dock-spaces/viewport/GizmoSettingsPanel.h
        src/context/editor/dock-spaces/viewport/PaintingSettingsPanel.cpp
        src/context/editor/dock-spaces/viewport/PaintingSettingsPanel.h
        src/context/editor/abstract/form/types/QuatField.cpp
        src/context/editor/abstract/form/types/QuatField.h
        src/service/transform/TransformService.cpp
        src/service/transform/TransformService.h
        src/repository/world/impl/WorldGridRepository.cpp
        src/repository/world/impl/WorldGridRepository.h
        src/repository/world/impl/WorldTile.h
        src/repository/world/impl/BoundingBox.h
        src/service/voxel/impl/SparseVoxelOctreeBuilder.cpp
        src/service/voxel/impl/SparseVoxelOctreeBuilder.h
        src/service/voxel/impl/OctreeNode.cpp
        src/service/voxel/impl/OctreeNode.h
        src/service/voxel/impl/VoxelData.h
        src/service/voxel/VoxelizationService.cpp
        src/service/voxel/VoxelizationService.h
        src/service/mesh/SceneData.h
        src/service/mesh/EntityAssetData.h
        src/service/mesh/MaterialData.h
        src/context/engine/render-pass/impl/PostProcessingPass.cpp
        src/context/engine/render-pass/impl/PostProcessingPass.h
        src/dto/push-constant/PostProcessingPushConstant.h
        src/context/engine/render-pass/impl/GBufferShadingPass.cpp
        src/context/engine/render-pass/impl/GBufferShadingPass.h
        src/dto/push-constant/GBufferShadingPushConstant.h
        src/service/pipeline/PipelineBuilder.h
        src/service/framebuffer/FrameBufferAttachment.cpp
        src/repository/engine/EngineRepository.cpp
        src/repository/engine/EngineRepository.h
        src/repository/texture/CoreTextures.cpp
        src/repository/texture/CoreTextures.h
        src/util/serialization-definitions.h
        src/service/notification/NotificationService.cpp
        src/service/notification/NotificationService.h
        src/dto/Notification.h
        src/enum/NotificationSeverity.h
        src/service/voxel/impl/Triangle.h
        src/service/voxel/impl/SparseVoxelOctreeData.h
        src/context/engine/render-pass/impl/tools/VoxelVisualizerPass.cpp
        src/context/engine/render-pass/impl/tools/VoxelVisualizerPass.h
        src/service/voxel/SVOService.cpp
        src/service/voxel/SVOService.h
        src/service/voxel/SVOInstance.cpp
        src/service/voxel/SVOInstance.h
        src/dto/push-constant/VoxelDebugSettingsPushConstant.h
        src/service/world/WorldGridService.cpp
        src/service/world/WorldGridService.h
        src/dto/ubo/TileInfoUBO.h
        src/dto/ubo/LightData.h
        src/repository/world/components/LightComponent.cpp
        src/repository/world/components/LightComponent.h
        src/context/engine/render-pass/impl/tools/IconsPass.cpp
        src/context/engine/render-pass/impl/tools/IconsPass.h
        src/dto/push-constant/IconPushConstant.h
        src/context/engine/render-pass/impl/GlobalIlluminationPass.cpp
        src/context/engine/render-pass/impl/GlobalIlluminationPass.h
        src/repository/editor/EditorRepository.cpp
        src/dto/push-constant/GIPushConstant.h
        src/dto/push-constant/GridPushConstant.h
        src/context/engine/render-pass/impl/AtmospherePass.cpp
        src/context/engine/render-pass/impl/AtmospherePass.h
        src/context/engine/compute-pass/AbstractComputePass.cpp
        src/context/engine/compute-pass/AbstractComputePass.h
        src/context/engine/passes/AbstractPass.cpp
        src/context/engine/passes/AbstractPass.h
        src/context/engine/passes/PassesService.cpp
        src/context/engine/passes/PassesService.h
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
)
target_link_libraries(metal_engine PRIVATE ${COMMON_LIBRARIES})
target_include_directories(metal_engine PRIVATE dependencies/stb)
add_subdirectory(dependencies/vk-bootstrap)
# ------ LIBS

# ------ STATIC RESOURCES
file(COPY "${CMAKE_SOURCE_DIR}/resources" DESTINATION "${CMAKE_BINARY_DIR}/")
# ------ STATIC RESOURCES

# ------ VCPKG
set(CMAKE_TOOLCHAIN_FILE "/Users/gustavoroque/vcpkg/scripts/buildsystems/vcpkg.cmake")
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