cmake_minimum_required(VERSION 3.30)
project(metal_engine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")

add_executable(
        metal_engine
        src/main.cpp
        src/editor/EditorPanel.cpp
        src/editor/EditorPanel.h
        src/editor/common/IPanel.h
        src/engine/EngineContext.cpp
        src/engine/EngineContext.h
        src/common/util/VulkanUtils.cpp
        src/common/util/VulkanUtils.h
        src/context/VulkanContext.cpp
        src/context/VulkanContext.h
        src/context/GLFWContext.cpp
        src/context/GLFWContext.h
        src/context/GuiContext.cpp
        src/context/GuiContext.h
        src/editor/ui/viewport/ViewportPanel.cpp
        src/editor/ui/viewport/ViewportPanel.h
        src/editor/ui/docks/AbstractDockPanel.cpp
        src/editor/ui/docks/AbstractDockPanel.h
        src/context/ApplicationContext.cpp
        src/context/ApplicationContext.h
        src/editor/common/AbstractPanel.h
        src/editor/common/AbstractPanel.cpp
        src/common/interface/Synchornizable.h
        src/engine/service/camera/CameraService.cpp
        src/engine/service/camera/CameraService.h
        src/engine/service/camera/Camera.h
        src/common/util/Util.h
        src/common/inspection/Inspectable.h
        src/common/inspection/Inspectable.cpp
        src/common/inspection/InspectedField.h
        src/editor/ui/inspector/InspectorPanel.cpp
        src/editor/ui/inspector/InspectorPanel.h
        src/editor/common/form/FormPanel.cpp
        src/editor/common/form/FormPanel.h
        src/editor/common/form/AccordionPanel.cpp
        src/editor/common/form/AccordionPanel.h
        src/common/inspection/FieldType.h
        src/common/inspection/InspectableMember.h
        src/common/interface/AbstractRuntimeComponent.h
        src/common/interface/AbstractRepository.h
        src/common/interface/AbstractRepository.cpp
        src/editor/common/form/types/BooleanField.cpp
        src/editor/common/form/types/BooleanField.h
        src/editor/common/form/types/IntField.cpp
        src/editor/common/form/types/IntField.h
        src/editor/common/form/types/FloatField.cpp
        src/editor/common/form/types/FloatField.h
        src/editor/common/form/types/MethodField.cpp
        src/editor/common/form/types/MethodField.h
        src/common/util/UIUtil.h
        src/editor/ui/header/EditorHeaderPanel.cpp
        src/editor/ui/header/EditorHeaderPanel.h
        src/editor/ui/header/GlobalSettingsPanel.cpp
        src/editor/ui/header/GlobalSettingsPanel.h
        src/common/interface/Icons.h
        src/editor/service/DockService.cpp
        src/editor/service/DockService.h
        src/editor/EditorContext.cpp
        src/editor/EditorContext.h
        src/editor/repository/dock/DockRepository.cpp
        src/editor/repository/dock/DockRepository.h
        src/editor/repository/dock/DockDTO.h
        src/common/interface/MEnum.h
        src/editor/repository/dock/DockSpace.h
        src/editor/repository/dock/DockPosition.h
        src/editor/ui/docks/DockSpacePanel.cpp
        src/editor/ui/docks/DockSpacePanel.h
        src/editor/repository/dock/DockSpace.cpp
        src/editor/service/ThemeService.cpp
        src/editor/service/ThemeService.h
        src/editor/repository/EditorRepository.cpp
        src/editor/repository/EditorRepository.h
        src/engine/engine-definitions.h
        src/editor/ui/metrics/MetricsPanel.cpp
        src/editor/ui/metrics/MetricsPanel.h
        src/editor/ui/files/FilesPanel.cpp
        src/editor/ui/files/FilesPanel.h
        src/editor/ui/console/ConsolePanel.cpp
        src/editor/ui/console/ConsolePanel.h
        src/editor/ui/hierarchy/HierarchyPanel.cpp
        src/editor/ui/hierarchy/HierarchyPanel.h
        src/context/runtime/RuntimeResource.h
        src/context/service/ShaderUtil.cpp
        src/context/service/ShaderUtil.h
        src/context/runtime/ShaderModule.cpp
        src/context/runtime/ShaderModule.h
        src/engine/render-pass/AbstractRenderPass.cpp
        src/engine/render-pass/AbstractRenderPass.h
        src/engine/render-pass/impl/OpaqueRenderPass.cpp
        src/engine/render-pass/impl/OpaqueRenderPass.h
        src/context/runtime/TextureInstance.h
        src/context/repository/ResourceRepository.h
        src/common/interface/AbstractResourceService.cpp
        src/common/interface/AbstractResourceService.h
        src/context/runtime/FrameBufferAttachment.h
        src/context/repository/CoreFrameBuffers.cpp
        src/context/repository/CoreFrameBuffers.h
        src/context/repository/AbstractCoreRepository.h
        src/context/repository/AbstractCoreRepository.cpp
        src/context/repository/CorePipelines.cpp
        src/context/repository/CorePipelines.h
        src/context/service/FrameBufferService.cpp
        src/context/service/FrameBufferService.h
        src/context/runtime/FrameBufferInstance.h
        src/context/service/PipelineService.cpp
        src/context/service/PipelineService.h
        src/context/runtime/PipelineInstance.h
        src/context/repository/CoreBuffers.cpp
        src/context/repository/CoreBuffers.h
        src/context/service/BufferService.cpp
        src/context/service/BufferService.h
        src/context/service/DescriptorService.cpp
        src/context/service/DescriptorService.h
        src/context/runtime/DescriptorInstance.cpp
        src/context/runtime/DescriptorInstance.h
        src/context/buffers/GlobalDataUBO.h
        src/engine/RuntimeRepository.h
        src/context/repository/CoreDescriptorSets.cpp
        src/context/repository/CoreDescriptorSets.h
        src/context/runtime/BufferInstance.cpp
        src/context/runtime/BufferInstance.h
        src/context/runtime/PipelineInstance.cpp
        src/context/service/MeshService.cpp
        src/context/service/MeshService.h
        src/context/runtime/assets/MeshData.h
        src/context/runtime/MeshInstance.cpp
        src/context/runtime/MeshInstance.h
        src/common/util/files/FilesUtil.h
        src/common/util/files/FileEntry.h
        src/common/util/files/EntryType.h
        src/editor/ui/files/FilesContext.h
        src/common/util/files/FileDialogUtil.h
        src/editor/ui/files/FilesHeader.cpp
        src/editor/ui/files/FilesHeader.h
        src/editor/service/import/SceneImporter.cpp
        src/editor/service/import/SceneImporter.h
        src/editor/service/import/AbstractImporter.h
        src/engine/enum/LevelOfDetail.h
        src/engine/enum/LevelOfDetail.cpp
        src/editor/service/import/TextureImporter.cpp
        src/editor/service/import/TextureImporter.h
        src/common/util/files/FileMetadata.h
        src/context/runtime/assets/TextureData.h
        src/context/service/TextureService.cpp
        src/context/service/TextureService.h
        src/editor/service/FilesService.cpp
        src/editor/service/FilesService.h
        src/editor/ui/files/FilesContext.cpp
        src/context/buffers/MeshPushConstant.h
        src/context/runtime/assets/VertexData.h
        src/editor/ui/viewport/CameraPositionPanel.cpp
        src/editor/ui/viewport/CameraPositionPanel.h
        src/engine/render-pass/tools/GridRenderPass.cpp
        src/engine/render-pass/tools/GridRenderPass.h
        src/context/runtime/CommandBufferRecorder.cpp
        src/context/runtime/CommandBufferRecorder.h
        src/context/repository/CoreRenderPasses.cpp
        src/context/repository/CoreRenderPasses.h
        src/engine/service/camera/Camera.cpp
        src/engine/service/world/WorldRepository.cpp
        src/engine/service/world/WorldRepository.h
        src/engine/service/world/impl/Entity.h
        src/engine/enum/ComponentType.h
        src/engine/service/world/impl/AbstractComponent.h
        src/engine/service/world/components/MeshComponent.h
        src/engine/service/world/components/TransformComponent.h
        src/engine/enum/ShadingMode.h
        src/engine/enum/EditorMode.h
        src/editor/ui/hierarchy/HierarchyHeaderPanel.cpp
        src/editor/ui/hierarchy/HierarchyHeaderPanel.h
        src/editor/service/SelectionService.cpp
        src/editor/service/SelectionService.h
        src/engine/service/world/impl/Entity.cpp
        src/engine/service/world/components/TransformComponent.cpp
        src/engine/service/world/components/MeshComponent.cpp
        src/editor/common/form/types/ResourceField.cpp
        src/editor/common/form/types/ResourceField.h
        src/editor/common/form/types/ColorField.cpp
        src/editor/common/form/types/ColorField.h
        src/editor/common/form/types/Vec4Field.cpp
        src/editor/common/form/types/Vec4Field.h
        src/editor/common/form/types/Vec3Field.cpp
        src/editor/common/form/types/Vec3Field.h
        src/editor/common/form/types/Vec2Field.cpp
        src/editor/common/form/types/Vec2Field.h
        src/editor/common/form/types/StringField.cpp
        src/editor/common/form/types/StringField.h
        src/editor/common/form/types/ResourceFilesPanel.h
        src/editor/common/form/types/ResourceFilesPanel.cpp
        src/engine/service/streaming/StreamingRepository.cpp
        src/engine/service/streaming/StreamingRepository.h
        src/context/runtime/ResourceType.h
        src/editor/ui/viewport/GizmoPanel.cpp
        src/editor/ui/viewport/GizmoPanel.h
        src/editor/ui/viewport/ViewportHeaderPanel.cpp
        src/editor/ui/viewport/ViewportHeaderPanel.h
        src/editor/ui/viewport/GizmoSettingsPanel.cpp
        src/editor/ui/viewport/GizmoSettingsPanel.h
        src/editor/ui/viewport/PaintingSettingsPanel.cpp
        src/editor/ui/viewport/PaintingSettingsPanel.h
        src/editor/common/form/types/QuatField.cpp
        src/editor/common/form/types/QuatField.h
        src/engine/service/TransformService.cpp
        src/engine/service/TransformService.h
        src/engine/service/world/impl/WorldGridRepository.cpp
        src/engine/service/world/impl/WorldGridRepository.h
        src/engine/service/world/impl/WorldTile.h
        src/engine/service/world/impl/BoundingBox.h
        src/engine/service/voxel/impl/SparseVoxelOctree.cpp
        src/engine/service/voxel/impl/SparseVoxelOctree.h
        src/engine/service/voxel/impl/OctreeNode.cpp
        src/engine/service/voxel/impl/OctreeNode.h
        src/engine/service/voxel/impl/VoxelData.h
        src/engine/service/voxel/VoxelizerService.cpp
        src/engine/service/voxel/VoxelizerService.h
        src/context/runtime/assets/SceneData.h
        src/context/runtime/assets/EntityAssetData.h
        src/context/runtime/assets/MaterialData.h
        src/engine/render-pass/impl/PostProcessingPass.cpp
        src/engine/render-pass/impl/PostProcessingPass.h
        src/context/buffers/PPSettingsUBO.h
        src/engine/render-pass/impl/GBufferShadingPass.cpp
        src/engine/render-pass/impl/GBufferShadingPass.h
        src/context/buffers/GBufferShadingPushConstant.h
        src/context/service/PipelineBuilder.h
        src/context/runtime/FrameBufferAttachment.cpp
        src/engine/service/AtmosphereRepository.cpp
        src/engine/service/AtmosphereRepository.h
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