#ifndef METAL_ENGINE_EDITORCONTEXT_H
#define METAL_ENGINE_EDITORCONTEXT_H

#include "repository/EditorRepository.h"
#include "service/DockService.h"
#include "repository/dock/DockRepository.h"
#include "service/ThemeService.h"
#include "../common/interface/AbstractRuntimeComponent.h"

namespace Metal {
    class EditorContext final : public AbstractRuntimeComponent {
    public:
        ThemeService themeService{context};
        DockService dockService{context};
        DockRepository dockRepository{context};
        EditorRepository editorRepository{};

        explicit EditorContext(ApplicationContext &context);
    };
}

#endif
