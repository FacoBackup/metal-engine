#ifndef GIZMOSETTINGSPANEL_H
#define GIZMOSETTINGSPANEL_H
#include <imgui.h>

#include "../../abstract/AbstractPanel.h"

namespace Metal {
    struct EditorRepository;
}

namespace Metal {
    class GizmoSettingsPanel final : public AbstractPanel {
        static constexpr ImVec2 SPACING = ImVec2(0, 0);
        static constexpr const char *SNAP_ROTATE_OPTIONS = "5\0 10\0 15\0 30\0 45\0";
        static constexpr std::array<float, 5> SNAP_ROTATE_OPTIONS_A = {5, 10, 15, 30, 45};
        static constexpr const char *SNAP_TRANSLATE_OPTIONS = "0.5\0 1\0 2\0 5\0 10\0";
        static constexpr std::array<float, 5> SNAP_TRANSLATE_OPTIONS_A = {0.5, 1, 2, 5, 10};
        static constexpr const char *SNAP_SCALE_OPTIONS = "0.5\0 1\0 2\0 5\0 10\0";
        static constexpr std::array<float, 5> SNAP_SCALE_OPTIONS_A = {0.5, 1, 2, 5, 10};
        EditorRepository *editorRepository = nullptr;

    public:
        void onInitialize() override;

        void gizmoGrid() const;

        void gizmoMode() const;

        void gizmoSelection() const;

        void renderSnapTranslate() const;

        void renderSnapRotate() const;

        void renderSnapScale() const;

        void onSync() override;
    };
} // Metal

#endif //GIZMOSETTINGSPANEL_H
