#include "renderer/context/GUIContext.h"
#include "ui/EditorPanel.h"
#include <stdio.h>

int main(int, char **) {
    Metal::ApplicationContext context;
    if (!context.isValidContext()) {
        printf("GLFW: Vulkan Not Supported\n");
        return 1;
    }
    context.start();
    return 0;
}