cmake_minimum_required(VERSION 3.30)
project(metal_engine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")

add_subdirectory(dependencies/vk-bootstrap)

add_executable(
        metal_engine
        src/main.cpp
        src/renderer/IUIRenderer.h
        src/ui/EditorUIRenderer.cpp
        src/ui/EditorUIRenderer.h
        src/ui/IUIRenderer.h
        src/engine/Engine.cpp
        src/engine/Engine.h
        src/renderer/VulkanCoreData.h
        src/renderer/RendererCoreData.h
        src/renderer/VulkanUtils.cpp
        src/renderer/VulkanUtils.h
        src/renderer/RendererContext.cpp
        src/renderer/RendererContext.h
        src/renderer/context/VulkanContext.cpp
        src/renderer/context/VulkanContext.h
        src/renderer/context/GLFWContext.cpp
        src/renderer/context/GLFWContext.h
        src/renderer/context/GUIContext.cpp
        src/renderer/context/GUIContext.h)

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
