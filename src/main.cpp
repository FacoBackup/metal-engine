#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define VMA_IMPLEMENTATION
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "context/ApplicationContext.h"
#include "editor/EditorPanel.h"

int main(int, char **) {
    auto editorPanel = Metal::EditorPanel{};
    Metal::ApplicationContext context{editorPanel, true};
    editorPanel.setContext(&context);

    if (!context.isValidContext()) {
        printf("GLFW: Vulkan Not Supported\n");
        return 1;
    }
    context.start();
    return 0;
}
