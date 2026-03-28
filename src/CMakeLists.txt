# Include subdirectories
add_subdirectory(common)
add_subdirectory(core)
add_subdirectory(editor)
add_subdirectory(engine)

# Prefix sources from subdirectories
foreach (src ${COMMON_SOURCES})
    list(APPEND PREFIXED_COMMON_SOURCES "common/${src}")
endforeach ()

foreach (src ${CORE_SOURCES})
    list(APPEND PREFIXED_CORE_SOURCES "core/${src}")
endforeach ()

foreach (src ${EDITOR_SOURCES})
    list(APPEND PREFIXED_EDITOR_SOURCES "editor/${src}")
endforeach ()

foreach (src ${ENGINE_SOURCES_LIST})
    list(APPEND PREFIXED_ENGINE_SOURCES "engine/${src}")
endforeach ()

set(ENGINE_SOURCES
        ${PREFIXED_COMMON_SOURCES}
        ${PREFIXED_CORE_SOURCES}
        ${PREFIXED_EDITOR_SOURCES}
        ${PREFIXED_ENGINE_SOURCES}
        ApplicationContext.cpp
        ApplicationContext.h
        ApplicationEventContext.cpp
        ApplicationEventContext.h
)

# Extract non-main sources for testing
set(ENGINE_LIB_SOURCES ${ENGINE_SOURCES})

add_library(metal_engine_lib STATIC ${ENGINE_LIB_SOURCES})
target_include_directories(metal_engine_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(metal_engine_lib PUBLIC ${COMMON_LIBRARIES})

add_executable(metal_engine main.cpp)
target_link_libraries(metal_engine PRIVATE metal_engine_lib)
if (WIN32)
    target_link_libraries(metal_engine PRIVATE dwmapi)
endif ()
add_dependencies(metal_engine copy_resources)

add_custom_command(TARGET metal_engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/resources"
        "$<TARGET_FILE_DIR:metal_engine>/resources")
