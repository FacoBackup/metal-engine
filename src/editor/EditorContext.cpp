#include "EditorContext.h"

namespace Metal {
    EditorContext::EditorContext(ApplicationContext &context) : AbstractRuntimeComponent(context) {
    }

    void EditorContext::onInitialize() {
        filesService.onInitialize();
    }
} // Metal