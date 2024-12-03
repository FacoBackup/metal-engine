cmake_minimum_required(VERSION 3.30)
project(metal_engine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")

add_subdirectory(dependencies/vk-bootstrap)

add_executable(
        metal_engine
        src/main.cpp
        src/ui/EditorPanel.cpp
        src/ui/EditorPanel.h
        src/ui/common/IPanel.h
        src/engine/Engine.cpp
        src/engine/Engine.h
        src/renderer/VulkanUtils.cpp
        src/renderer/VulkanUtils.h
        src/renderer/context/VulkanContext.cpp
        src/renderer/context/VulkanContext.h
        src/renderer/context/GLFWContext.cpp
        src/renderer/context/GLFWContext.h
        src/renderer/context/GUIContext.cpp
        src/renderer/context/GUIContext.h
        src/ui/views/viewport/ViewportPanel.cpp
        src/ui/views/viewport/ViewportPanel.h
        src/ui/docks/AbstractDockSpacePanel.cpp
        src/ui/docks/AbstractDockSpacePanel.h
        src/context/ApplicationContext.cpp
        src/context/ApplicationContext.h
        src/ui/common/AbstractPanel.h
        src/ui/common/AbstractPanel.cpp
        src/common/Synchornizable.h
        src/engine/system/camera/CameraSystem.cpp
        src/engine/system/camera/CameraSystem.h
        src/engine/system/camera/CameraRepository.h
        src/engine/system/camera/Camera.h
        src/engine/system/camera/Frustum.cpp
        src/engine/system/camera/Frustum.h
        src/common/Util.h
        src/common/Identifiable.h
        src/common/inspection/Inspectable.h
        src/common/inspection/Inspectable.cpp
        src/common/inspection/InspectedField.h
        src/ui/views/inspector/InspectorPanel.cpp
        src/ui/views/inspector/InspectorPanel.h
        src/ui/common/form/FormPanel.cpp
        src/ui/common/form/FormPanel.h
        src/ui/common/form/AccordionPanel.cpp
        src/ui/common/form/AccordionPanel.h
        src/common/inspection/FieldType.h
        src/common/inspection/InspectableMember.h
)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Vulkan)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

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
)

target_link_libraries(metal_engine PRIVATE ${COMMON_LIBRARIES})

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
