#ifndef EDITORREPOSITORY_H
#define EDITORREPOSITORY_H
#include <imgui.h>
#include <ImGuizmo.h>
#include <vector>
#include <unordered_map>
#include <glm/vec3.hpp>

#include "../../engine/engine-definitions.h"
#include "../../engine/enum/EditorMode.h"
#include "../../engine/enum/ShadingMode.h"
#include "../../common/inspection/Inspectable.h"

namespace Metal {
    struct TransformComponent;

    struct EditorRepository final : Inspectable {
        bool showIcons = true;
        float iconScale = 1;
        glm::vec3 iconColor{1.f};

        glm::vec3 accentColor{0.26f, 0.59f, 0.98f};
        bool isDarkMode = true;

        bool showGrid = true;
        bool gridOverlayObjects = true;
        float gridScale = 1.f;
        int gridThreshold = 100;
        float gridThickness = 2;
        ImGuizmo::OPERATION gizmoType = ImGuizmo::OPERATION::TRANSLATE;
        ImGuizmo::MODE gizmoMode = ImGuizmo::MODE::WORLD;
        ImVec4 accent{};
        ImU32 accentU32 = 0;

        // Outline settings
        bool showOutline = true;
        float outlineWidth = 2.f;
        // Color outlineColor = new Color(1f, 0.5f, 0f);

        // ExecutionEnvironment environment = ExecutionEnvironment.DEVELOPMENT;
        // EditorMode editorMode = EditorMode.TRANSFORM;

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

        std::unordered_map<EntityID, bool> pinnedEntities{};
        TransformComponent *primitiveSelected = nullptr;
        EntityID mainSelection = EMPTY_ENTITY;
        std::unordered_map<EntityID, bool> selected{};
        std::vector<EntityID> copied{};

        // BrushMode brushMode = BrushMode.ADD;
        float brushRadius = 10;
        float brushDensity = .5f;
        EditorMode::EditorMode editorMode = EditorMode::EditorMode::TRANSFORM;
        ShadingMode::ShadingMode shadingMode = ShadingMode::ShadingMode::LIT;
        bool brushModeAdd = true;

        const char *getIcon() override;

        const char *getTitle() override;
    };
} // Metal

#endif
