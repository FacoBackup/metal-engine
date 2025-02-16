#ifndef BOTTOMLEVELACCELERATIONSTRUCTURE_H
#define BOTTOMLEVELACCELERATIONSTRUCTURE_H
#include <../../../../cmake-build-debug/vcpkg_installed/arm64-osx/include/glm/vec3.hpp>
#include <../../../../cmake-build-debug/vcpkg_installed/arm64-osx/include/glm/vec4.hpp>

namespace Metal {
    struct BottomLevelAccelerationStructure final {
        // stores max position of bounding box and the triangles offset stored on the triangles buffer
        alignas(16) glm::vec4 maxOffset;
        // stores min position of bounding box and the triangle count
        alignas(16) glm::vec4 minCount;
    };
}

#endif //BOTTOMLEVELACCELERATIONSTRUCTURE_H
