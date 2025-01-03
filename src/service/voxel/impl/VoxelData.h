#ifndef VOXELDATA_H
#define VOXELDATA_H
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#define IS_LEAF 1

namespace Metal {
    struct VoxelData {
        glm::ivec3 color;
        glm::vec2 roughnessMetallic;
        glm::vec3 normal;
        bool isEmissive;

        explicit VoxelData(const glm::ivec3 color, const glm::vec3 normal,
                           const glm::vec2 roughnessMetallic, bool isEmissive) : color(color),
            roughnessMetallic(roughnessMetallic),
            normal(normal), isEmissive(isEmissive) {
        }

        std::array<unsigned int, 2> compress() {
            const int red = (color.r / 2) & 0x7F; // Scale down and mask to 7 bits
            const int green = color.g & 0xFF; // No change, 8 bits
            const int blue = (color.b / 2) & 0x7F; // Scale down and mask to 7 bits
            const unsigned int albedo = (red << 16) | (green << 8) | blue;
            const unsigned int roughness = static_cast<unsigned int>(std::round(roughnessMetallic.r * 100)); // 0 to 100
            const unsigned int metallic = static_cast<unsigned int>(std::round(roughnessMetallic.g * 100));
            const unsigned int normalMetallic = CompressNormal(normal) << 7 | metallic;
            return std::array{
                (albedo << 9) | (isEmissive << 8) | roughness,
                normalMetallic
            };
        }

    private:
        static unsigned int CompressNormal(glm::vec3 &normal) {
            normal = glm::normalize(normal);
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
