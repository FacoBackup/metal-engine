cmake_minimum_required(VERSION 3.30)
project(metal_engine CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")

if (MSVC)
    add_compile_options(/utf-8)
endif()

# ------ LIBS
find_package(SDL3 REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)
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
find_package(EnTT CONFIG REQUIRED)

set(COMMON_LIBRARIES
        SDL3::SDL3
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
        nlohmann_json::nlohmann_json
        EnTT::EnTT

        glslang::glslang
        glslang::glslang-default-resource-limits
        glslang::SPVRemapper
        OpenVDB::openvdb
)

add_subdirectory(dependencies/vk-bootstrap)
# ------ LIBS

add_subdirectory(src)
enable_testing()
add_subdirectory(tests)

# ------ STATIC RESOURCES
file(GLOB_RECURSE RESOURCE_FILES "resources/*")
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/resources"
    "${CMAKE_BINARY_DIR}/resources"
    DEPENDS ${RESOURCE_FILES}
    COMMENT "Copying resources to build directory"
)
# ------ STATIC RESOURCES

# ------ VCPKG
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