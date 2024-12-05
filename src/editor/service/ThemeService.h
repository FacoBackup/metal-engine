#ifndef THEMESERVICE_H
#define THEMESERVICE_H
#include <imgui.h>
#include <glm/vec3.hpp>

#include "../../common/runtime/AbstractRuntimeComponent.h"

namespace Metal {
    class ThemeService final : public AbstractRuntimeComponent {
    public:
        ImVec4 neutralPalette{};
        ImVec4 palette0{};
        ImVec4 palette1{};
        ImVec4 palette2{};
        ImVec4 palette3{};
        ImVec4 palette4{};
        ImVec4 palette5{};
        ImVec4 palette6{};
        ImVec4 textDisabled{};
        static glm::vec3 BACKGROUND_COLOR;
        bool previousTheme = false;
        float prevLength = 0.0f;

        explicit ThemeService(ApplicationContext &context) : AbstractRuntimeComponent(context) {
        }

        void setDarkMode();

        void setLightMode();

        void onSync() override;
    };
} // Metal

#endif
