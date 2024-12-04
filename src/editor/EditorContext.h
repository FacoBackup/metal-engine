#ifndef METAL_ENGINE_EDITORCONTEXT_H
#define METAL_ENGINE_EDITORCONTEXT_H

#include "../common/runtime/AbstractRuntimeComponent.h"
#include "service/DockService.h"
#include "repository/dock/DockRepository.h"

namespace Metal {

    class EditorContext : public AbstractRuntimeComponent {
    public:
        DockService dockService{context};
        DockRepository dockRepository{context};

        explicit EditorContext(ApplicationContext &context);

    };

}

#endif
