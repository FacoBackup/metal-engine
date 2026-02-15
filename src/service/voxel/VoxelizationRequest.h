#ifndef VOXELIZATIONREQUEST_H
#define VOXELIZATIONREQUEST_H
#include <string>
#include <glm/mat4x4.hpp>

namespace Metal {
    struct VoxelizationRequest final {
        const glm::mat4x4 &transform;
        const MeshComponent &meshComponent;
    };
}

#endif //VOXELIZATIONREQUEST_H
