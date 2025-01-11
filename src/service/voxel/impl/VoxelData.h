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
                           const float roughness, bool isEmissive) {
            const auto roughnessLocal = static_cast<unsigned int>(std::round(roughness * 100)); // 0 to 100
            int red = (color.r / 2) & 0xFF;
            int green = color.g & 0xFF;
            int blue = (color.b / 2) & 0xFF;
            unsigned int albedo = (red << 16) | (green << 8) | blue;

            data = std::array{
                isEmissive << 31 | roughnessLocal << 24 | albedo,
                CompressNormal(normal)
            };
        }

    private:
        // THANKS TO https://www.shadertoy.com/view/llfcRl
        static unsigned int CompressNormal(const glm::vec3 &nor) {
            glm::vec3 mor = nor;
            uint32_t id = 0;

            if (std::abs(nor.y) > std::abs(mor.x)) {
                mor = glm::vec3(nor.y, nor.z, nor.x);
                id = 1u;
            }
            if (std::abs(nor.z) > std::abs(mor.x)) {
                mor = glm::vec3(nor.z, nor.x, nor.y);
                id = 2u;
            }

            uint32_t is = (mor.x < 0.0f) ? 1u : 0u;
            glm::vec2 uv = 0.5f + 0.5f * glm::vec2(mor.y, mor.z) / std::abs(mor.x);
            glm::uvec2 iuv = glm::uvec2(glm::round(uv * glm::vec2(32767.0f, 16383.0f)));

            return iuv.x | (iuv.y << 15u) | (id << 29u) | (is << 31u);
        }
    };
}

#endif //VOXELDATA_H
