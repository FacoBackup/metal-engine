#ifndef EDITORREPOSITORY_H
#define EDITORREPOSITORY_H
#include <imgui.h>
#include <ImGuizmo.h>
#include <vector>
#include <unordered_map>
#include <glm/vec3.hpp>

#include "../../common/serialization-definitions.h"
#include "../../common/engine-definitions.h"
#include "../../common/enum/ShadingMode.h"
#include "../../common/inspection/Inspectable.h"
#include "../data/NavigationPosition.h"
#include "../data/Views.h"

namespace Metal {
    struct TransformComponent;

    struct ViewInstance final {
        unsigned int viewIndex;
        float viewRatio;
        bool initialized = false;

        SERIALIZE_TEMPLATE(viewIndex, viewRatio, initialized)
    };

    struct EditorRepository final : Inspectable {
        ImVec4 accent{};
        ImU32 accentU32 = 0;
        glm::vec3 accentColor{0.26f, 0.59f, 0.98f};
        std::unordered_map<EntityID, bool> selected{};
        std::unordered_map<NavigationPosition, std::array<ViewInstance, 2> > views{};

        explicit EditorRepository() {
            views.insert({
                RIGHT,
                {
                    {
                        {Views::FindIndexByName(Views::HIERARCHY), .65, true},
                        {Views::FindIndexByName(Views::INSPECTOR), .35, true}
                    }
                }
            });
            views.insert({
                BOTTOM,
                {
                    {
                        {Views::FindIndexByName(Views::CONSOLE), .5, true},
                        {Views::FindIndexByName(Views::FILES), .5, true}
                    }
                }
            });
            views.insert({
                LEFT, {}
            });
        }

        float topBlockRatio = .75;
        float rightBlockSize = 300;
        float leftBlockSize = 300;

        std::string projectName = "New project";
        bool showIcons = true;

        bool isDarkMode = false;
        bool isCompactMode = false;

        bool showGrid = true;
        float gridScale = 1.f;
        int gridThreshold = 100;
        float gridThickness = .1;
        ImGuizmo::OPERATION gizmoType = ImGuizmo::OPERATION::TRANSLATE;
        ImGuizmo::MODE gizmoMode = ImGuizmo::MODE::WORLD;
        float gizmoSnapRotate = 0.f;
        float gizmoSnapScale = 0.f;
        float gizmoSnapTranslate = 0.f;
        int gizmoSnapTranslateOption = 0;
        int gizmoSnapRotateOption = 0;
        int gizmoSnapScaleOption = 0;
        bool gizmoUseSnapTranslate = false;
        bool gizmoUseSnapRotate = false;
        bool gizmoUseSnapScale = false;
        bool showOnlyEntitiesHierarchy = false;
        TransformComponent *primitiveSelected = nullptr;
        EntityID mainSelection = EMPTY_ENTITY;

        std::vector<EntityID> copied{};
        ShadingMode::ShadingMode shadingMode = ShadingMode::LIT;

        void registerFields() override;

        const char *getTitle() override;

        const char *getIcon() override;

        SAVE_TEMPLATE(
            accentColor.x, accentColor.y, accentColor.z,
            projectName, showIcons,
            isDarkMode,
            showGrid,
            gridScale,
            gridThreshold,
            gridThickness,
            gizmoType,
            gizmoMode,
            gizmoSnapRotate,
            gizmoSnapScale,
            gizmoSnapTranslate,
            gizmoSnapTranslateOption,
            gizmoSnapRotateOption,
            gizmoSnapScaleOption,
            gizmoUseSnapTranslate,
            gizmoUseSnapRotate,
            gizmoUseSnapScale,
            showOnlyEntitiesHierarchy,
            mainSelection,
            selected,
            copied,
            shadingMode,
            views
        )
    };
} // Metal

#endif
