#ifndef MATERIALINSPECTION_H
#define MATERIALINSPECTION_H
#include "../../abstract/AbstractPanel.h"
#include "../../abstract/form/FormPanel.h"

namespace Metal {
    struct MaterialInfo;

    class MaterialInspection final : public AbstractPanel {
        std::string prevSelection;
        MaterialInfo *data = nullptr;
        FormPanel *formPanel = nullptr;
        long long sinceLastChange = 0;

    public:
        void onInitialize() override;

        void saveChanges();

        void onSync() override;
    };
} // Metal

#endif //MATERIALINSPECTION_H
