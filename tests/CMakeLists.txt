find_package(GTest REQUIRED)

add_executable(metal_engine_tests
        ApplicationContextTest.cpp
        WorldRepositoryTest.cpp
)

target_link_libraries(metal_engine_tests
        PRIVATE
        metal_engine_lib
        GTest::gtest_main
)
add_dependencies(metal_engine_tests copy_resources)

add_custom_command(TARGET metal_engine_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/resources"
        "$<TARGET_FILE_DIR:metal_engine_tests>/resources")

include(GoogleTest)
gtest_discover_tests(metal_engine_tests)
