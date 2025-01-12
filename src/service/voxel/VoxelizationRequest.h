#ifndef VOXELIZATIONREQUEST_H
#define VOXELIZATIONREQUEST_H
#include <string>
#include <glm/mat4x4.hpp>

namespace Metal {
    struct VoxelizationRequest final {
        glm::mat4x4 &transform;
        MeshComponent &meshComponent;
    };
}

#endif //VOXELIZATIONREQUEST_H
