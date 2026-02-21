#ifndef MATERIALINSPECTION_H
#define MATERIALINSPECTION_H
#include "../../abstract/AbstractPanel.h"
#include "../../abstract/form/FormPanel.h"

namespace Metal {
    struct MaterialFileData;

    class MaterialInspection final : public AbstractPanel {
        std::string prevSelection;
        MaterialFileData *data = nullptr;
        FormPanel *formPanel = nullptr;
    public:
        void onInitialize() override;

        void saveChanges();

        void onSync() override;
    };
} // Metal

#endif //MATERIALINSPECTION_H
