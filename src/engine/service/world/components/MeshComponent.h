#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H
#include "../impl/AbstractComponent.h"

namespace Metal {
    struct MeshComponent final : AbstractComponent {
        const char *meshId = nullptr;

        const char *getIcon() override;

        const char *getTitle() override;

        void registerFields() override;
    };
}
#endif //MESHCOMPONENT_H
