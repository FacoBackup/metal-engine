#include "backends-impl/metal/MetalRenderer.h"

int main() {
    backends::metal::MetalRenderer renderer;
    renderer.start();
    renderer.shutdown();
    return 0;
}
