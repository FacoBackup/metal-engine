#ifndef VIEWPORTHEADERPANEL_H
#define VIEWPORTHEADERPANEL_H
#include <imgui.h>

#include "editor/ui/abstract/AbstractPanel.h"

namespace Metal {
    struct EditorRepository;
    struct CameraRepository;

    class ViewportHeaderPanel final : public AbstractPanel {
        static constexpr int FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
                                     ImGuiWindowFlags_NoCollapse;
        std::shared_ptr<AbstractPanel> gizmo = nullptr;
        int shadingModelOption = 0;

        EditorRepository *editorRepository = nullptr;
        CameraRepository *cameraRepository = nullptr;

    public:
        std::vector<Dependency> getDependencies() override {
            return {
                {"EditorRepository", &editorRepository},
                {"CameraRepository", &cameraRepository}
            };
        }

        static constexpr float HEIGHT = 28;

        void onInitialize() override;

        void onSync() override;

        void cameraMode() const;

        void shadingMode();
    };
} // Metal

#endif //VIEWPORTHEADERPANEL_H
