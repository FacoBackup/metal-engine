#ifndef VOXELDATA_H
#define VOXELDATA_H
#include <iostream>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace Metal {
    struct VoxelData {
        std::array<unsigned int, 2> data{};

        explicit VoxelData(const glm::ivec3 color, glm::vec3 normal,
                           const glm::vec2 roughnessMetallic, bool isEmissive) {
            const auto roughness = static_cast<unsigned int>(std::round(roughnessMetallic.r * 100)); // 0 to 100
            const auto metallic = static_cast<unsigned int>(std::round(roughnessMetallic.g * 100));
            const unsigned int normalMetallic = CompressNormal(normal) << 7 | metallic;

            int red = (color.r / 2) & 0xFF;
            int green = color.g & 0xFF;
            int blue = (color.b / 2) & 0xFF;
            unsigned int albedo = (red << 16) | (green << 8) | blue;

            data = std::array{
                isEmissive << 31 | roughness << 24 | albedo,
                normalMetallic
            };
        }

    private:
        static unsigned int CompressNormal(glm::vec3 &normal) {
            // Octahedral mapping
            glm::vec2 oct = glm::vec2(normal.x, normal.y) / (
                                std::abs(normal.x) + std::abs(normal.y) + std::abs(normal.z));
            if (normal.z < 0) {
                oct = (1.0f - glm::abs(oct)) * glm::sign(oct);
            }

            // Quantize to a 12-bit range for each component
            int quantX = static_cast<int>((oct.x * 2047.0f) + 2048.0f);
            int quantY = static_cast<int>((oct.y * 2047.0f) + 2048.0f);

            // Pack into a 25-bit integer
            return (quantX << 13) | quantY;
        }
    };
}

#endif //VOXELDATA_H
