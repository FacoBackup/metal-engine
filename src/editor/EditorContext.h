#ifndef METAL_ENGINE_EDITORCONTEXT_H
#define METAL_ENGINE_EDITORCONTEXT_H

#include "repository/EditorRepository.h"
#include "service/DockService.h"
#include "repository/dock/DockRepository.h"
#include "service/ThemeService.h"
#include "../common/interface/AbstractRuntimeComponent.h"
#include "repository/FilesService.h"
#include "service/SelectionService.h"
#include "service/import/MeshImporter.h"
#include "service/import/TextureImporter.h"

namespace Metal {
    class EditorContext final : public AbstractRuntimeComponent {
    public:
        ThemeService themeService{context};
        DockService dockService{context};
        DockRepository dockRepository{context};
        EditorRepository editorRepository{};
        SelectionService selectionService{context};

        // ------ IMPORTERS
        MeshImporter meshImporter{context};
        TextureImporter textureImporter{context};
        // ------ IMPORTERS

        FilesService filesService{context};

        explicit EditorContext(ApplicationContext &context);

        void onInitialize() override;
    };
}

#endif
