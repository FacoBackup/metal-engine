#ifndef MATERIALINSPECTION_H
#define MATERIALINSPECTION_H
#include "../../abstract/AbstractPanel.h"
#include "../../abstract/form/FormPanel.h"

namespace Metal {
    struct MaterialData;

    class MaterialInspection final : public AbstractPanel {
        std::string prevSelection;
        MaterialData *data = nullptr;
        FormPanel *formPanel = nullptr;
    public:
        void onInitialize() override;

        void onSync() override;
    };
} // Metal

#endif //MATERIALINSPECTION_H
