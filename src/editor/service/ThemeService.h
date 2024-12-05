#ifndef THEMESERVICE_H
#define THEMESERVICE_H
#include "../../common/runtime/AbstractRuntimeComponent.h"

namespace Metal {
    class ThemeService final : public AbstractRuntimeComponent {
    public:
        explicit ThemeService(ApplicationContext &context) : AbstractRuntimeComponent(context) {
        }

        void onSync() override;
    };
} // Metal

#endif
