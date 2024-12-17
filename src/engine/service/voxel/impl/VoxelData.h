#ifndef VOXELDATA_H
#define VOXELDATA_H
#include <glm/vec3.hpp>

namespace Metal {
    struct VoxelData {
        glm::ivec3 color;

        explicit VoxelData(const glm::ivec3 color) : color(color) {
        }

        int compress() const {
            const int red = (color.r / 2) & 0x7F; // Scale down and mask to 7 bits
            const int green = color.g & 0xFF; // No change, 8 bits
            const int blue = (color.b / 2) & 0x7F; // Scale down and mask to 7 bits

            return (red << 16) | (green << 8) | blue;
        }
    };
}

#endif //VOXELDATA_H
