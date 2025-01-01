#ifndef EDITORREPOSITORY_H
#define EDITORREPOSITORY_H
#include <imgui.h>
#include <ImGuizmo.h>
#include <vector>
#include <unordered_map>
#include <glm/vec3.hpp>

#include "../../util/serialization-definitions.h"
#include "../../enum/engine-definitions.h"
#include "../../enum/EditorMode.h"
#include "../../enum/ShadingMode.h"
#include "../../common/inspection/Inspectable.h"

namespace Metal {
    struct TransformComponent;

    struct EditorRepository final : Inspectable {
        bool showVoxels = false;
        bool showRaySearchCountVoxels = true;
        bool showRayTestCountVoxels = true;
        int voxelSearchCount = 32;

        ImVec4 accent{};
        ImU32 accentU32 = 0;

        glm::vec3 iconColor{1.f};
        glm::vec3 accentColor{0.26f, 0.59f, 0.98f};
        std::string projectName = "New project";
        bool showIcons = true;
        float iconScale = 1;
        bool isDarkMode = true;
        bool showGrid = true;
        bool gridOverlayObjects = false;
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
        std::unordered_map<EntityID, bool> pinnedEntities{};
        TransformComponent *primitiveSelected = nullptr;
        EntityID mainSelection = EMPTY_ENTITY;
        std::unordered_map<EntityID, bool> selected{};
        std::vector<EntityID> copied{};
        EditorMode::EditorMode editorMode = EditorMode::EditorMode::TRANSFORM;
        ShadingMode::ShadingMode shadingMode = ShadingMode::ShadingMode::LIT;
        bool brushModeAdd = true;
        float brushRadius = 10;
        float brushDensity = .5f;

        void registerFields() override;

        const char *getTitle() override;

        const char *getIcon() override;

        SAVE_TEMPLATE(
            iconColor.x, iconColor.y, iconColor.z,
            accentColor.x, accentColor.y, accentColor.z,
            projectName, showIcons, iconScale,
            isDarkMode,
            showGrid,
            gridOverlayObjects,
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
            pinnedEntities,
            mainSelection,
            selected,
            copied,
            editorMode,
            shadingMode,
            brushModeAdd,
            brushRadius,
            brushDensity,
            showRaySearchCountVoxels,
            showRayTestCountVoxels
        )
    };
} // Metal

#endif
