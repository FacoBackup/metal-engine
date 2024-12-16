#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H
#include "../impl/AbstractComponent.h"

namespace Metal {
    struct MeshComponent final : AbstractComponent {
        std::string meshId;

        const char *getIcon() override;

        const char *getTitle() override;

        void registerFields() override;
    };
}
#endif //MESHCOMPONENT_H
