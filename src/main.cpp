#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define VMA_IMPLEMENTATION
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
