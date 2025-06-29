#ifndef TEXTUREDATA_H
#define TEXTUREDATA_H

#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

namespace Metal {
    struct TextureData final {
        int width;
        int height;
        int channels;
        unsigned char *data;

        glm::vec4 sampleAt(glm::vec2 &uv) const {
            glm::vec2 pixelCoords = uvToPixelCoords(uv);
            const int x = pixelCoords[0];
            const int y = pixelCoords[1];

            const int pixelIndex = (y * width + x) * channels;
            const int r = (data[pixelIndex] & 0xFF);
            const int g = (data[pixelIndex + 1] & 0xFF);
            const int b = (data[pixelIndex + 2] & 0xFF);
            int a = 1;
            if (channels == 4) {
                a = (data[pixelIndex + 3] & 0xFF);
            }
            return glm::vec4(r, g, b, a);
        }

    private:
        glm::vec2 uvToPixelCoords(glm::vec2 &uv) const {
            // Clamp UV coordinates to ensure they are within [0,1]
            uv.r = std::max(0.f, std::min(uv.r, 1.f));
            uv.g = std::max(0.f, std::min(uv.g, 1.f));

            // Correct horizontal flipping by flipping the U coordinate
            int x = static_cast<int>(uv.r * (width - 1));
            int y = static_cast<int>(uv.g * (height - 1));

            return glm::vec2{x, y};
        }
    };
}
#endif //TEXTUREDATA_H
