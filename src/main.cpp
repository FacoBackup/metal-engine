#include "common/runtime/ApplicationContext.h"
#include "editor/EditorPanel.h"

int main(int, char **) {
    auto editorPanel = Metal::EditorPanel{};
    Metal::ApplicationContext context{editorPanel};
    editorPanel.setContext(&context);

    if (!context.isValidContext()) {
        printf("GLFW: Vulkan Not Supported\n");
        return 1;
    }
    context.start();
    return 0;
}
