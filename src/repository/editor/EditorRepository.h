#ifndef EDITORREPOSITORY_H
#define EDITORREPOSITORY_H
#include <imgui.h>
#include <ImGuizmo.h>
#include <vector>
#include <unordered_map>
#include <glm/vec3.hpp>

#include "../../dto/ShortcutDTO.h"
#include "../../enum/engine-definitions.h"
#include "../../enum/ShadingMode.h"
#include "../../common/inspection/Inspectable.h"
#include "../../util/Serializable.h"

namespace Metal {
    struct TransformComponent;

    struct EditorRepository final : Inspectable, Serializable {
        bool showRaySearchCountVoxels = false;
        bool showRayTestCountVoxels = false;
        int voxelSearchCount = 32;

        ImVec4 accent{};
        ImU32 accentU32 = 0;

        glm::vec3 accentColor{0.26f, 0.59f, 0.98f};
        glm::vec3 selectionColor{1, 1, 0};
        int selectionOutlineThickness = 3;
        std::string projectName = "New project";
        bool showIcons = true;
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
        TransformComponent *primitiveSelected = nullptr;
        EntityID mainSelection = EMPTY_ENTITY;
        std::unordered_map<EntityID, bool> selected{};
        std::vector<EntityID> copied{};
        std::string focusedWindowName{};
        std::vector<ShortcutDTO> focusedShortcuts{};
        ShadingMode::ShadingMode shadingMode = ShadingMode::ShadingMode::LIT;
        bool brushModeAdd = true;
        float brushRadius = 10;
        float brushDensity = .5f;

        void registerFields() override;

        const char *getTitle() override;

        const char *getIcon() override;

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["accentColor"] = {accentColor.x, accentColor.y, accentColor.z};
            j["selectionColor"] = {selectionColor.x, selectionColor.y, selectionColor.z};
            j["selectionOutlineThickness"] = selectionOutlineThickness;
            j["projectName"] = projectName;
            j["showIcons"] = showIcons;
            j["isDarkMode"] = isDarkMode;
            j["showGrid"] = showGrid;
            j["gridOverlayObjects"] = gridOverlayObjects;
            j["gridScale"] = gridScale;
            j["gridThreshold"] = gridThreshold;
            j["gridThickness"] = gridThickness;
            j["gizmoType"] = (int)gizmoType;
            j["gizmoMode"] = (int)gizmoMode;
            j["gizmoSnapRotate"] = gizmoSnapRotate;
            j["gizmoSnapScale"] = gizmoSnapScale;
            j["gizmoSnapTranslate"] = gizmoSnapTranslate;
            j["gizmoSnapTranslateOption"] = gizmoSnapTranslateOption;
            j["gizmoSnapRotateOption"] = gizmoSnapRotateOption;
            j["gizmoSnapScaleOption"] = gizmoSnapScaleOption;
            j["gizmoUseSnapTranslate"] = gizmoUseSnapTranslate;
            j["gizmoUseSnapRotate"] = gizmoUseSnapRotate;
            j["gizmoUseSnapScale"] = gizmoUseSnapScale;
            j["showOnlyEntitiesHierarchy"] = showOnlyEntitiesHierarchy;
            j["mainSelection"] = mainSelection;
            j["selected"] = selected;
            j["copied"] = copied;
            j["shadingMode"] = (int)shadingMode;
            j["brushModeAdd"] = brushModeAdd;
            j["brushRadius"] = brushRadius;
            j["brushDensity"] = brushDensity;
            j["showRaySearchCountVoxels"] = showRaySearchCountVoxels;
            j["showRayTestCountVoxels"] = showRayTestCountVoxels;
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            accentColor = {j.at("accentColor")[0], j.at("accentColor")[1], j.at("accentColor")[2]};
            selectionColor = {j.at("selectionColor")[0], j.at("selectionColor")[1], j.at("selectionColor")[2]};
            selectionOutlineThickness = j.at("selectionOutlineThickness").get<int>();
            projectName = j.at("projectName").get<std::string>();
            showIcons = j.at("showIcons").get<bool>();
            isDarkMode = j.at("isDarkMode").get<bool>();
            showGrid = j.at("showGrid").get<bool>();
            gridOverlayObjects = j.at("gridOverlayObjects").get<bool>();
            gridScale = j.at("gridScale").get<float>();
            gridThreshold = j.at("gridThreshold").get<int>();
            gridThickness = j.at("gridThickness").get<float>();
            gizmoType = (ImGuizmo::OPERATION)j.at("gizmoType").get<int>();
            gizmoMode = (ImGuizmo::MODE)j.at("gizmoMode").get<int>();
            gizmoSnapRotate = j.at("gizmoSnapRotate").get<float>();
            gizmoSnapScale = j.at("gizmoSnapScale").get<float>();
            gizmoSnapTranslate = j.at("gizmoSnapTranslate").get<float>();
            gizmoSnapTranslateOption = j.at("gizmoSnapTranslateOption").get<int>();
            gizmoSnapRotateOption = j.at("gizmoSnapRotateOption").get<int>();
            gizmoSnapScaleOption = j.at("gizmoSnapScaleOption").get<int>();
            gizmoUseSnapTranslate = j.at("gizmoUseSnapTranslate").get<bool>();
            gizmoUseSnapRotate = j.at("gizmoUseSnapRotate").get<bool>();
            gizmoUseSnapScale = j.at("gizmoUseSnapScale").get<bool>();
            showOnlyEntitiesHierarchy = j.at("showOnlyEntitiesHierarchy").get<bool>();
            mainSelection = j.at("mainSelection").get<EntityID>();
            selected = j.at("selected").get<std::unordered_map<EntityID, bool>>();
            copied = j.at("copied").get<std::vector<EntityID>>();
            shadingMode = (ShadingMode::ShadingMode)j.at("shadingMode").get<int>();
            brushModeAdd = j.at("brushModeAdd").get<bool>();
            brushRadius = j.at("brushRadius").get<float>();
            brushDensity = j.at("brushDensity").get<float>();
            showRaySearchCountVoxels = j.at("showRaySearchCountVoxels").get<bool>();
            showRayTestCountVoxels = j.at("showRayTestCountVoxels").get<bool>();
        }
    };
} // Metal

#endif
