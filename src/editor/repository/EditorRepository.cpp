#include "EditorRepository.h"
#include "common/Icons.h"

namespace Metal {
    const char *EditorRepository::getTitle() const {
        return "Editor";
    }

    const char *EditorRepository::getIcon() const {
        return Icons::settings.c_str();
    }

    void EditorRepository::registerFields() {
        registerEditableField(&accentColor, COLOR, "accentColor", "Editor");
        registerEditableField(&selectionColor, COLOR, "selectionColor", "Selection outline");
        registerEditableField(&selectionOutlineThickness, INT, "selectionOutlineThickness", "Selection outline");
        registerEditableField(&projectName, STRING, "projectName", "Project");
        registerEditableField(&showIcons, BOOLEAN, "showIcons", "Icons");
        registerEditableField(&isDarkMode, BOOLEAN, "isDarkMode", "Editor");
        registerEditableField(&showGrid, BOOLEAN, "showGrid", "Grid");
        registerEditableField(&gridOverlayObjects, BOOLEAN, "gridOverlayObjects", "Grid");
        registerEditableField(&gridScale, FLOAT, "gridScale", "Grid");
        registerEditableField(&gridThreshold, INT, "gridThreshold", "Grid");
        registerEditableField(&gridThickness, FLOAT, "gridThickness", "Grid");
        registerSerializableOnlyField(&gizmoType, INT, "gizmoType").setTransformer(
            [this] {
                return static_cast<int>(gizmoType);
            },
            [this](const nlohmann::json &j) {
                gizmoType = static_cast<ImGuizmo::OPERATION>(j.get<int>());
            });
        registerSerializableOnlyField(&gizmoMode, INT, "gizmoMode").setTransformer(
            [this] {
                return static_cast<int>(gizmoMode);
            },
            [this](const nlohmann::json &j) {
                gizmoMode = static_cast<ImGuizmo::MODE>(j.get<int>());
            });
        registerSerializableOnlyField(&gizmoSnapRotate, FLOAT, "gizmoSnapRotate");
        registerSerializableOnlyField(&gizmoSnapScale, FLOAT, "gizmoSnapScale");
        registerSerializableOnlyField(&gizmoSnapTranslate, FLOAT, "gizmoSnapTranslate");
        registerSerializableOnlyField(&gizmoSnapTranslateOption, INT, "gizmoSnapTranslateOption");
        registerSerializableOnlyField(&gizmoSnapRotateOption, INT, "gizmoSnapRotateOption");
        registerSerializableOnlyField(&gizmoSnapScaleOption, INT, "gizmoSnapScaleOption");
        registerSerializableOnlyField(&gizmoUseSnapTranslate, BOOLEAN, "gizmoUseSnapTranslate");
        registerSerializableOnlyField(&gizmoUseSnapRotate, BOOLEAN, "gizmoUseSnapRotate");
        registerSerializableOnlyField(&gizmoUseSnapScale, BOOLEAN, "gizmoUseSnapScale");
        registerSerializableOnlyField(&showOnlyEntitiesHierarchy, BOOLEAN, "showOnlyEntitiesHierarchy");
        registerSerializableOnlyField(&mainSelection, INT, "mainSelection").setTransformer(
            [this] {
                return static_cast<uint32_t>(mainSelection);
            },
            [this](const nlohmann::json &j) {
                mainSelection = static_cast<entt::entity>(j.get<uint32_t>());
            });
        registerSerializableOnlyField(&selected, INT, "selected").setTransformer(
            [this] {
                nlohmann::json j = nlohmann::json::array();
                for (auto const &[key, val]: selected) {
                    j.push_back(static_cast<uint32_t>(key));
                }
                return j;
            },
            [this](const nlohmann::json &j) {
                selected.clear();
                for (auto const &item: j) {
                    selected[static_cast<entt::entity>(item.get<uint32_t>())] = true;
                }
            });
        registerSerializableOnlyField(&copied, INT, "copied").setTransformer(
            [this] {
                nlohmann::json j = nlohmann::json::array();
                for (auto const &entity: copied) {
                    j.push_back(static_cast<uint32_t>(entity));
                }
                return j;
            },
            [this](const nlohmann::json &j) {
                copied.clear();
                for (auto const &item: j) {
                    copied.push_back(static_cast<entt::entity>(item.get<uint32_t>()));
                }
            });
        registerSerializableOnlyField(&shadingMode, INT, "shadingMode").setTransformer(
            [this] {
                return static_cast<int>(shadingMode);
            },
            [this](const nlohmann::json &j) {
                shadingMode = static_cast<ShadingMode>(j.get<int>());
            });
        registerSerializableOnlyField(&bookmarks, STRING, "bookmarks"); // JSON supports vector<string> directly
        registerSerializableOnlyField(&isPlaying, BOOLEAN, "isPlaying");
        registerEditableField(&gptMcpKey, STRING, "gptMcpKey", "MCP");
        registerEditableField(&geminiMcpKey, STRING, "geminiMcpKey", "MCP");
    }
}
