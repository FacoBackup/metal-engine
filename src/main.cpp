#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define VMA_IMPLEMENTATION
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "MetalContextProvider.h"
#include "common/Logger.h"

int main(int, char **) {
    Logger::Init();
    Metal::ContextProvider::Initialize(true);

    return 0;
}