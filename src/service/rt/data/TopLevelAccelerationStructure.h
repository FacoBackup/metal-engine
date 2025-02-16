#ifndef TOPLEVELACCELERATIONSTRUCTURE_H
#define TOPLEVELACCELERATIONSTRUCTURE_H
#include <../../../../cmake-build-debug/vcpkg_installed/arm64-osx/include/glm/mat4x4.hpp>

namespace Metal {
    struct TopLevelAccelerationStructure final {
        alignas(16)glm::mat4x4 transform;
        alignas(4) unsigned int bottomLevelASOffset;
    };
}
#endif //TOPLEVELACCELERATIONSTRUCTURE_H
