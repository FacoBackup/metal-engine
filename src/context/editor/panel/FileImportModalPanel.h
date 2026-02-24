#ifndef METAL_ENGINE_FILEIMPORTMODALPANEL_H
#define METAL_ENGINE_FILEIMPORTMODALPANEL_H
#include "../abstract/AbstractPanel.h"

namespace Metal {
    class FormPanel;

    class FileImportModalPanel final : public AbstractPanel {
        FormPanel *settingsForm = nullptr;
        bool isFirst = false;

    public:
        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif //METAL_ENGINE_FILEIMPORTMODALPANEL_H
