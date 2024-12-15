#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H
#include "AbstractComponent.h"
#include "../../../../common/interface/Icons.h"

namespace Metal {
    struct MeshComponent final : AbstractComponent {
        const char *meshId = nullptr;

        const char *getIcon() override {
            return Icons::category.c_str();
        }

        const char *getTitle() override {
            return "Mesh Component";
        }
    };
}
#endif //MESHCOMPONENT_H
