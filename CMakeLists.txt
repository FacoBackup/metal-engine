cmake_minimum_required(VERSION 3.30)
project(metal_engine)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        metal_engine
        MACOSX_BUNDLE
        src/main.mm
        src/backends-impl/IUIRenderer.h
        src/backends-impl/metal/MetalRenderer.h
        src/backends-impl/metal/MetalRenderer.mm)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_library(METAL_FRAMEWORK Metal)
find_library(METALKIT_FRAMEWORK MetalKit)
find_library(COCOA Cocoa)
find_library(IO_KIT IOKit)
find_library(CORE_VIDEO CoreVideo)
find_library(QUARTZ QuartzCore)

set(COMMON_LIBRARIES
        glfw
        glm::glm
        imgui::imgui
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        ${METAL_FRAMEWORK}
        ${METALKIT_FRAMEWORK}
        ${COCOA}
        ${IO_KIT}
        ${CORE_VIDEO}
        ${QUARTZ}
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
