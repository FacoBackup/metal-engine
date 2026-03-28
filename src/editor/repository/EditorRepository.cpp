#include "EditorRepository.h"
#include "common/Icons.h"

namespace Metal {
    const char *EditorRepository::getTitle() const {
        return "Editor";
    }

    const char *EditorRepository::getIcon() const {
        return Icons::settings.c_str();
    }

    void EditorRepository::clear() {
        selected.clear();
        copied.clear();
        bookmarks.clear();
        mainSelection = EMPTY_ENTITY;
    }

    void EditorRepository::registerFields() {
        registerEditableField<COLOR>(&accentColor).setName("accentColor").setLabel("Accent color");
        registerEditableField<COLOR>(&selectionColor).setName("selectionColor").setLabel("Selection outline color");
        registerEditableField<INT>(&selectionOutlineThickness).setName("selectionOutlineThickness").setLabel("Selection outline thickness");
        registerEditableField<STRING>(&projectName).setName("projectName").setLabel("Project name");
        registerEditableField<BOOLEAN>(&showIcons).setName("showIcons").setLabel("Show icons");
        registerEditableField<BOOLEAN>(&isDarkMode).setName("isDarkMode").setLabel("Dark mode");
        registerEditableField<BOOLEAN>(&showGrid).setName("showGrid").setLabel("Show grid");
        registerEditableField<BOOLEAN>(&gridOverlayObjects).setName("gridOverlayObjects").setLabel("Grid overlay objects");
        registerEditableField<FLOAT>(&gridScale).setName("gridScale").setLabel("Grid scale");
        registerEditableField<INT>(&gridThreshold).setName("gridThreshold").setLabel("Grid threshold");
        registerEditableField<FLOAT>(&gridThickness).setName("gridThickness").setLabel("Grid thickness");
        auto gizmoTypeToJson = [this] { return static_cast<int>(gizmoType); };
        auto gizmoTypeFromJson = [this](const nlohmann::json &j) { gizmoType = static_cast<ImGuizmo::OPERATION>(j.get<int>()); };
        registerGenericField(gizmoTypeToJson, gizmoTypeFromJson).setName("gizmoType");
        auto gizmoModeToJson = [this] { return static_cast<int>(gizmoMode); };
        auto gizmoModeFromJson = [this](const nlohmann::json &j) { gizmoMode = static_cast<ImGuizmo::MODE>(j.get<int>()); };
        registerGenericField(gizmoModeToJson, gizmoModeFromJson).setName("gizmoMode");
        registerSerializableOnlyField<FLOAT>(&gizmoSnapRotate).setName("gizmoSnapRotate");
        registerSerializableOnlyField<FLOAT>(&gizmoSnapScale).setName("gizmoSnapScale");
        registerSerializableOnlyField<FLOAT>(&gizmoSnapTranslate).setName("gizmoSnapTranslate");
        registerSerializableOnlyField<INT>(&gizmoSnapTranslateOption).setName("gizmoSnapTranslateOption");
        registerSerializableOnlyField<INT>(&gizmoSnapRotateOption).setName("gizmoSnapRotateOption");
        registerSerializableOnlyField<INT>(&gizmoSnapScaleOption).setName("gizmoSnapScaleOption");
        registerSerializableOnlyField<BOOLEAN>(&gizmoUseSnapTranslate).setName("gizmoUseSnapTranslate");
        registerSerializableOnlyField<BOOLEAN>(&gizmoUseSnapRotate).setName("gizmoUseSnapRotate");
        registerSerializableOnlyField<BOOLEAN>(&gizmoUseSnapScale).setName("gizmoUseSnapScale");
        registerSerializableOnlyField<BOOLEAN>(&showOnlyEntitiesHierarchy).setName("showOnlyEntitiesHierarchy");
        auto mainSelectionToJson = [this] { return static_cast<uint32_t>(mainSelection); };
        auto mainSelectionFromJson = [this](const nlohmann::json &j) { mainSelection = static_cast<entt::entity>(j.get<uint32_t>()); };
        registerGenericField(mainSelectionToJson, mainSelectionFromJson).setName("mainSelection");
        auto selectedToJson = [this] {
            nlohmann::json j = nlohmann::json::array();
            for (auto const &[key, val]: selected) {
                j.push_back(static_cast<uint32_t>(key));
            }
            return j;
        };
        auto selectedFromJson = [this](const nlohmann::json &j) {
            selected.clear();
            for (auto const &item: j) {
                selected[static_cast<entt::entity>(item.get<uint32_t>())] = true;
            }
        };
        registerGenericField(selectedToJson, selectedFromJson).setName("selected");
        auto copiedToJson = [this] {
            nlohmann::json j = nlohmann::json::array();
            for (auto const &entity: copied) {
                j.push_back(static_cast<uint32_t>(entity));
            }
            return j;
        };
        auto copiedFromJson = [this](const nlohmann::json &j) {
            copied.clear();
            for (auto const &item: j) {
                copied.push_back(static_cast<entt::entity>(item.get<uint32_t>()));
            }
        };
        registerGenericField(copiedToJson, copiedFromJson).setName("copied");
        auto shadingModeToJson = [this] { return static_cast<int>(shadingMode); };
        auto shadingModeFromJson = [this](const nlohmann::json &j) { shadingMode = static_cast<ShadingMode>(j.get<int>()); };
        registerGenericField(shadingModeToJson, shadingModeFromJson).setName("shadingMode");
        auto bookmarksToJson = [this] { return bookmarks; };
        auto bookmarksFromJson = [this](const nlohmann::json &j) { bookmarks = j.get<std::vector<std::string>>(); };
        registerGenericField(bookmarksToJson, bookmarksFromJson).setName("bookmarks");
        registerSerializableOnlyField<BOOLEAN>(&isPlaying).setName("isPlaying");
        registerEditableField<STRING>(&gptMcpKey).setName("gptMcpKey").setLabel("GPT API Key");
        registerEditableField<STRING>(&geminiMcpKey).setName("geminiMcpKey").setLabel("Gemini API Key");
        registerEditableField<BOOLEAN>(&enableSnapshots).setName("enableSnapshots").setLabel("Enable snapshots");
        registerEditableField<FLOAT>(&snapshotInterval).setName("snapshotInterval").setLabel("Snapshot interval");
    }
}
