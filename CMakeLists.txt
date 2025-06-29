cmake_minimum_required(VERSION 3.30)
project(metal_engine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")

add_executable(
        metal_engine
        src/main.cpp
        src/ui/editor/EditorPanel.cpp
        src/ui/editor/EditorPanel.h
        src/ui/editor/abstract/IPanel.h
        src/runtime/EngineContext.cpp
        src/runtime/EngineContext.h
        src/renderer/util/VulkanUtils.cpp
        src/renderer/util/VulkanUtils.h
        src/renderer/VulkanContext.cpp
        src/renderer/VulkanContext.h
        src/ui/GLFWContext.cpp
        src/ui/GLFWContext.h
        src/ui/GuiContext.cpp
        src/ui/GuiContext.h
        src/ui/editor/dock-spaces/viewport/ViewportPanel.cpp
        src/ui/editor/dock-spaces/viewport/ViewportPanel.h
        src/ui/editor/dock-spaces/docks/AbstractDockPanel.h
        src/context/ApplicationContext.cpp
        src/context/ApplicationContext.h
        src/ui/editor/abstract/AbstractPanel.h
        src/ui/editor/abstract/AbstractPanel.cpp
        src/common/Synchornizable.h
        src/runtime/service/CameraService.cpp
        src/runtime/service/CameraService.h
        src/runtime/data/Camera.h
        src/common/Util.h
        src/common/inspection/Inspectable.h
        src/common/inspection/Inspectable.cpp
        src/common/inspection/InspectedField.h
        src/ui/editor/dock-spaces/inspector/InspectorPanel.cpp
        src/ui/editor/dock-spaces/inspector/InspectorPanel.h
        src/ui/editor/abstract/form/FormPanel.cpp
        src/ui/editor/abstract/form/FormPanel.h
        src/ui/editor/abstract/form/AccordionPanel.cpp
        src/ui/editor/abstract/form/AccordionPanel.h
        src/common/enum/FieldType.h
        src/common/inspection/InspectableMember.h
        src/common/AbstractRuntimeComponent.h
        src/ui/editor/abstract/form/types/BooleanField.cpp
        src/ui/editor/abstract/form/types/BooleanField.h
        src/ui/editor/abstract/form/types/IntField.cpp
        src/ui/editor/abstract/form/types/IntField.h
        src/ui/editor/abstract/form/types/FloatField.cpp
        src/ui/editor/abstract/form/types/FloatField.h
        src/ui/editor/abstract/form/types/MethodField.cpp
        src/ui/editor/abstract/form/types/MethodField.h
        src/ui/util/UIUtil.h
        src/ui/editor/dock-spaces/header/EditorHeaderPanel.cpp
        src/ui/editor/dock-spaces/header/EditorHeaderPanel.h
        src/ui/editor/dock-spaces/header/GlobalSettingsPanel.cpp
        src/ui/editor/dock-spaces/header/GlobalSettingsPanel.h
        src/common/enum/Icons.h
        src/ui/service/DockService.cpp
        src/ui/service/DockService.h
        src/ui/repository/DockRepository.cpp
        src/ui/repository/DockRepository.h
        src/ui/data/dock/DockDTO.h
        src/ui/data/dock/DockSpace.h
        src/ui/data/dock/DockPosition.h
        src/ui/editor/dock-spaces/docks/DockSpacePanel.cpp
        src/ui/editor/dock-spaces/docks/DockSpacePanel.h
        src/ui/data/dock/DockSpace.cpp
        src/ui/service/ThemeService.cpp
        src/ui/service/ThemeService.h
        src/ui/repository/EditorRepository.h
        src/common/engine-definitions.h
        src/ui/editor/dock-spaces/metrics/MetricsPanel.cpp
        src/ui/editor/dock-spaces/metrics/MetricsPanel.h
        src/ui/editor/dock-spaces/files/FilesPanel.cpp
        src/ui/editor/dock-spaces/files/FilesPanel.h
        src/ui/editor/dock-spaces/console/ConsolePanel.cpp
        src/ui/editor/dock-spaces/console/ConsolePanel.h
        src/ui/editor/dock-spaces/hierarchy/HierarchyPanel.cpp
        src/ui/editor/dock-spaces/hierarchy/HierarchyPanel.h
        src/common/RuntimeResource.h
        src/renderer/util/ShaderUtil.cpp
        src/renderer/util/ShaderUtil.h
        src/renderer/data/ShaderModule.cpp
        src/renderer/data/ShaderModule.h
        src/renderer/passes/AbstractRenderPass.cpp
        src/renderer/passes/AbstractRenderPass.h
        src/renderer/data/TextureInstance.h
        src/common/ResourceRepository.h
        src/common/AbstractResourceService.cpp
        src/common/AbstractResourceService.h
        src/renderer/data/FrameBufferAttachment.h
        src/renderer/repository/CoreFrameBuffers.cpp
        src/renderer/repository/CoreFrameBuffers.h
        src/renderer/service/FrameBufferService.cpp
        src/renderer/service/FrameBufferService.h
        src/renderer/data/FrameBufferInstance.h
        src/renderer/service/PipelineService.cpp
        src/renderer/service/PipelineService.h
        src/renderer/data/PipelineInstance.h
        src/renderer/repository/CoreBuffers.cpp
        src/renderer/repository/CoreBuffers.h
        src/renderer/service/BufferService.cpp
        src/renderer/service/BufferService.h
        src/renderer/service/DescriptorService.cpp
        src/renderer/service/DescriptorService.h
        src/renderer/data/DescriptorInstance.cpp
        src/renderer/data/DescriptorInstance.h
        src/renderer/data/GlobalDataUBO.h
        src/runtime/repository/RuntimeRepository.h
        src/renderer/repository/CoreDescriptorSets.cpp
        src/renderer/repository/CoreDescriptorSets.h
        src/renderer/data/BufferInstance.cpp
        src/renderer/data/BufferInstance.h
        src/renderer/data/PipelineInstance.cpp
        src/renderer/service/MeshService.cpp
        src/renderer/service/MeshService.h
        src/renderer/data/MeshData.h
        src/common/FilesUtil.h
        src/common/FileEntry.h
        src/common/enum/EntryType.h
        src/ui/editor/dock-spaces/files/FilesContext.h
        src/ui/util/FileDialogUtil.h
        src/ui/editor/dock-spaces/files/FilesHeader.cpp
        src/ui/editor/dock-spaces/files/FilesHeader.h
        src/renderer/service/SceneImporterService.cpp
        src/renderer/service/SceneImporterService.h
        src/common/AbstractImporter.h
        src/common/enum/LevelOfDetail.h
        src/common/enum/LevelOfDetail.cpp
        src/renderer/service/TextureImporterService.cpp
        src/renderer/service/TextureImporterService.h
        src/common/FileMetadata.h
        src/renderer/data/TextureData.h
        src/renderer/service/TextureService.cpp
        src/renderer/service/TextureService.h
        src/ui/service/FilesService.cpp
        src/ui/service/FilesService.h
        src/ui/editor/dock-spaces/files/FilesContext.cpp
        src/renderer/data/VertexData.h
        src/ui/editor/dock-spaces/viewport/CameraPositionPanel.cpp
        src/ui/editor/dock-spaces/viewport/CameraPositionPanel.h
        src/renderer/passes/impl/tools/GridPass.cpp
        src/renderer/passes/impl/tools/GridPass.h
        src/renderer/passes/CommandBufferRecorder.cpp
        src/renderer/passes/CommandBufferRecorder.h
        src/runtime/data/Camera.cpp
        src/runtime/service/WorldRepository.cpp
        src/runtime/service/WorldRepository.h
        src/runtime/data/Entity.h
        src/common/enum/ComponentType.h
        src/runtime/data/components/AbstractComponent.h
        src/runtime/data/components/MeshComponent.h
        src/runtime/data/components/TransformComponent.h
        src/common/enum/ShadingMode.h
        src/ui/service/SelectionService.cpp
        src/ui/service/SelectionService.h
        src/runtime/data/Entity.cpp
        src/runtime/data/components/TransformComponent.cpp
        src/runtime/data/components/MeshComponent.cpp
        src/ui/editor/abstract/form/types/ResourceField.cpp
        src/ui/editor/abstract/form/types/ResourceField.h
        src/ui/editor/abstract/form/types/ColorField.cpp
        src/ui/editor/abstract/form/types/ColorField.h
        src/ui/editor/abstract/form/types/Vec4Field.cpp
        src/ui/editor/abstract/form/types/Vec4Field.h
        src/ui/editor/abstract/form/types/Vec3Field.cpp
        src/ui/editor/abstract/form/types/Vec3Field.h
        src/ui/editor/abstract/form/types/Vec2Field.cpp
        src/ui/editor/abstract/form/types/Vec2Field.h
        src/ui/editor/abstract/form/types/StringField.cpp
        src/ui/editor/abstract/form/types/StringField.h
        src/ui/editor/abstract/form/types/ResourceFilesPanel.h
        src/ui/editor/abstract/form/types/ResourceFilesPanel.cpp
        src/renderer/service/StreamingService.cpp
        src/renderer/service/StreamingService.h
        src/common/enum/ResourceType.h
        src/ui/editor/dock-spaces/viewport/GizmoPanel.cpp
        src/ui/editor/dock-spaces/viewport/GizmoPanel.h
        src/ui/editor/dock-spaces/viewport/GizmoSettingsPanel.cpp
        src/ui/editor/dock-spaces/viewport/GizmoSettingsPanel.h
        src/ui/editor/abstract/form/types/QuatField.cpp
        src/ui/editor/abstract/form/types/QuatField.h
        src/runtime/service/TransformService.cpp
        src/runtime/service/TransformService.h
        src/runtime/data/BoundingBox.h
        src/renderer/data/SceneData.h
        src/renderer/data/EntityAssetData.h
        src/renderer/passes/impl/PostProcessingPass.cpp
        src/renderer/passes/impl/PostProcessingPass.h
        src/renderer/data/PipelineBuilder.h
        src/renderer/data/FrameBufferAttachment.cpp
        src/runtime/repository/EngineRepository.cpp
        src/runtime/repository/EngineRepository.h
        src/renderer/repository/CoreTextures.cpp
        src/renderer/repository/CoreTextures.h
        src/common/serialization-definitions.h
        src/ui/service/NotificationService.cpp
        src/ui/service/NotificationService.h
        src/ui/data/Notification.h
        src/common/enum/NotificationSeverity.h
        src/renderer/data/LightBuffer.h
        src/runtime/data/components/LightComponent.cpp
        src/runtime/data/components/LightComponent.h
        src/renderer/passes/impl/tools/IconsPass.cpp
        src/renderer/passes/impl/tools/IconsPass.h
        src/ui/repository/EditorRepository.cpp
        src/renderer/data/GridPC.h
        src/renderer/passes/AbstractComputePass.cpp
        src/renderer/passes/AbstractComputePass.h
        src/renderer/passes/AbstractPass.cpp
        src/renderer/passes/AbstractPass.h
        src/renderer/service/PassesService.cpp
        src/renderer/service/PassesService.h
        src/renderer/util/ImageUtils.h
        src/renderer/service/MaterialService.cpp
        src/renderer/service/MaterialService.h
        src/ui/repository/FileInspectionRepository.cpp
        src/ui/repository/FileInspectionRepository.h
        src/ui/editor/dock-spaces/inspector/MaterialInspection.cpp
        src/ui/editor/dock-spaces/inspector/MaterialInspection.h
        src/common/enum/LightVolumeType.h
        src/renderer/passes/impl/AccumulationMetadataPass.cpp
        src/renderer/passes/impl/AccumulationMetadataPass.h
        src/renderer/passes/impl/AccumulationPass.cpp
        src/renderer/passes/impl/AccumulationPass.h
        src/renderer/service/LightsService.cpp
        src/renderer/service/LightsService.h
        src/runtime/data/components/VolumeComponent.cpp
        src/runtime/data/components/VolumeComponent.h
        src/renderer/data/DescriptorBinding.h
        src/renderer/data/TLASBuffer.h
        src/renderer/data/BLASBuffer.h
        src/renderer/data/TriangleBuffer.h
        src/renderer/service/BVHBuilderService.cpp
        src/renderer/service/BVHBuilderService.h
        src/renderer/data/BVH.h
        src/renderer/passes/impl/RTPass.cpp
        src/renderer/passes/impl/RTPass.h
        src/renderer/data/BoundingBoxBVH.h
        src/renderer/data/TriangleBVH.h
        src/renderer/data/MeshBVH.cpp
        src/renderer/data/MeshBVH.h
        src/renderer/data/MaterialInfo.h
        src/renderer/data/MaterialInfo.cpp
        src/renderer/data/MaterialInfoData.h
        src/renderer/service/VolumeService.cpp
        src/renderer/service/VolumeService.h
        src/ui/data/dock/DockViewDTO.h
        src/common/Logger.h
        src/MetalContextProvider.h
        src/MetalContextProvider.cpp
        src/context/ApplicationSingletons.h
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
set(CMAKE_TOOLCHAIN_FILE "C:/Users/cafe/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake")
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