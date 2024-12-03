#include "renderer/context/GUIContext.h"
#include "ui/EditorPanel.h"
#include <stdio.h>

int main(int, char **) {
    Metal::ApplicationContext context;

    context.getGuiContext().build(true);
    if (!context.getGuiContext().getWindowContext().isValidContext()) {
        printf("GLFW: Vulkan Not Supported\n");
        return 1;
    }
    context.getGuiContext().start();
    context.getGuiContext().shutdown();
    return 0;
}