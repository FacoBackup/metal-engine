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
        src/common/runtime/VulkanContext.cpp
        src/common/runtime/VulkanContext.h
        src/common/runtime/GLFWContext.cpp
        src/common/runtime/GLFWContext.h
        src/common/runtime/GUIContext.cpp
        src/common/runtime/GUIContext.h
        src/editor/ui/viewport/ViewportPanel.cpp
        src/editor/ui/viewport/ViewportPanel.h
        src/editor/ui/docks/AbstractDockPanel.cpp
        src/editor/ui/docks/AbstractDockPanel.h
        src/common/runtime/ApplicationContext.cpp
        src/common/runtime/ApplicationContext.h
        src/editor/common/AbstractPanel.h
        src/editor/common/AbstractPanel.cpp
        src/common/interface/Synchornizable.h
        src/engine/system/camera/CameraSystem.cpp
        src/engine/system/camera/CameraSystem.h
        src/engine/repository/camera/CameraRepository.h
        src/engine/repository/camera/Camera.h
        src/engine/repository/camera/Frustum.cpp
        src/engine/repository/camera/Frustum.h
        src/common/util/Util.h
        src/common/interface/Identifiable.h
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
        src/engine/repository/camera/CameraRepository.cpp
        src/common/interface/AbstractRepository.cpp
        src/editor/common/form/types/BooleanField.cpp
        src/editor/common/form/types/BooleanField.h
        src/editor/common/form/types/IntField.cpp
        src/editor/common/form/types/IntField.h
        src/editor/common/form/types/FloatField.cpp
        src/editor/common/form/types/FloatField.h
        src/editor/common/form/types/MethodField.cpp
        src/editor/common/form/types/MethodField.h
        src/editor/common/UIUtil.h
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
        src/editor/repository/dock/DockDTO.cpp
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
        src/common/interface/EntityID.h
        src/editor/ui/metrics/MetricsPanel.cpp
        src/editor/ui/metrics/MetricsPanel.h
        src/editor/ui/files/FilesPanel.cpp
        src/editor/ui/files/FilesPanel.h
        src/editor/ui/console/ConsolePanel.cpp
        src/editor/ui/console/ConsolePanel.h
        src/editor/ui/hierarchy/HierarchyPanel.cpp
        src/editor/ui/hierarchy/HierarchyPanel.h
        src/engine/repository/resource/RuntimeResource.h
        src/engine/service/core/shader/ShaderService.cpp
        src/engine/service/core/shader/ShaderService.h
        src/engine/service/core/shader/ShaderModuleInstance.cpp
        src/engine/service/core/shader/ShaderModuleInstance.h
        src/common/runtime/VulkanFrameData.cpp
        src/common/runtime/VulkanFrameData.h
        src/engine/render-pass/AbstractRenderPass.cpp
        src/engine/render-pass/AbstractRenderPass.h
        src/engine/system/RenderPassSystem.cpp
        src/engine/system/RenderPassSystem.h
        src/engine/render-pass/impl/VoxelVisualizerRenderPass.cpp
        src/engine/render-pass/impl/VoxelVisualizerRenderPass.h
        src/engine/service/core/texture/TextureService.cpp
        src/engine/service/core/texture/TextureService.h
        src/engine/service/core/texture/TextureInstance.h
        src/engine/repository/resource/ResourceRepository.h
        src/engine/service/AbstractResourceService.cpp
        src/engine/service/AbstractResourceService.h
        src/engine/service/core/framebuffer/FrameBufferAttachment.h
        src/engine/core/CoreFrameBuffers.cpp
        src/engine/core/CoreFrameBuffers.h
        src/engine/core/AbstractCoreRepository.h
        src/engine/core/AbstractCoreRepository.cpp
        src/engine/core/CorePipelines.cpp
        src/engine/core/CorePipelines.h
        src/engine/service/core/framebuffer/FrameBufferService.cpp
        src/engine/service/core/framebuffer/FrameBufferService.h
        src/engine/service/core/framebuffer/FrameBufferInstance.h
        src/engine/service/core/pipeline/PipelineService.cpp
        src/engine/service/core/pipeline/PipelineService.h
        src/engine/service/core/pipeline/PipelineInstance.h
        src/engine/repository/backend/CommandPoolRepository.cpp
        src/engine/repository/backend/CommandPoolRepository.h
        src/engine/service/core/pools/CommandPoolService.cpp
        src/engine/service/core/pools/CommandPoolService.h
        src/engine/core/CoreBuffers.cpp
        src/engine/core/CoreBuffers.h
        src/engine/service/core/buffer/BufferService.cpp
        src/engine/service/core/buffer/BufferService.h
        src/engine/service/core/buffer/BufferInstance.h
        src/engine/service/core/buffer/BufferType.h
        src/engine/service/core/pools/CommandBufferInstance.h
        src/engine/service/core/pools/CommandBufferInstance.cpp
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
set(COMMON_LIBRARIES
        glfw
        Vulkan::Headers
        vk-bootstrap
        vk-bootstrap-compiler-warnings
        GPUOpen::VulkanMemoryAllocator
        glm::glm
        imgui::imgui
        spdlog::spdlog
        nlohmann_json::nlohmann_json

        glslang::glslang
        glslang::glslang-default-resource-limits
        glslang::SPVRemapper
)
target_link_libraries(metal_engine PRIVATE ${COMMON_LIBRARIES})
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