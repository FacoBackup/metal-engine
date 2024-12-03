#include "context/ApplicationContext.h"

int main(int, char **) {
    Metal::ApplicationContext context;
    if (!context.isValidContext()) {
        printf("GLFW: Vulkan Not Supported\n");
        return 1;
    }
    context.start();
    return 0;
}
