#include "../../../repository/editor/EditorRepository.h"
#include "../../../common/Icons.h"

namespace Metal {
    const char *EditorRepository::getTitle() {
        return "Editor";
    }

    const char *EditorRepository::getIcon() {
        return Icons::settings.c_str();
    }

    nlohmann::json EditorRepository::toJson() const {
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
        j["gizmoType"] = gizmoType;
        j["gizmoMode"] = gizmoMode;
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
        j["mainSelection"] = static_cast<uint32_t>(entt::to_integral(mainSelection));
        j["selected"] = nlohmann::json::array();
        for (auto const& [key, val] : selected) {
            j["selected"].push_back(static_cast<uint32_t>(entt::to_integral(key)));
        }
        j["copied"] = nlohmann::json::array();
        for (auto const& entity : copied) {
            j["copied"].push_back(static_cast<uint32_t>(entt::to_integral(entity)));
        }
        j["shadingMode"] = shadingMode;
        return j;
    }

    void EditorRepository::fromJson(const nlohmann::json &j) {
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
            gizmoType = static_cast<ImGuizmo::OPERATION>(j.at("gizmoType").get<int>());
            gizmoMode = static_cast<ImGuizmo::MODE>(j.at("gizmoMode").get<int>());
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
            mainSelection = static_cast<entt::entity>(j.at("mainSelection").get<uint32_t>());
            selected.clear();
            if (j.contains("selected") && j.at("selected").is_array()) {
                for (auto const& item : j.at("selected")) {
                    selected[static_cast<entt::entity>(item.get<uint32_t>())] = true;
                }
            }
            copied.clear();
            if (j.contains("copied") && j.at("copied").is_array()) {
                for (auto const& item : j.at("copied")) {
                    copied.push_back(static_cast<entt::entity>(item.get<uint32_t>()));
                }
            }
            shadingMode = static_cast<ShadingMode>(j.at("shadingMode").get<int>());
    }

    void EditorRepository::registerFields() {
        registerColor(accentColor, "Editor", "Accent color");
        registerColor(selectionColor, "Selection outline", "Color");
        registerInt(selectionOutlineThickness, "Selection outline", "thickness");

        registerBool(showIcons, "Icons", "Show icons?");
        registerBool(showGrid, "Grid", "Enabled?");
        registerBool(gridOverlayObjects, "Grid", "Overlay objects?");
        registerFloat(gridScale, "Grid", "Scale");
        registerInt(gridThreshold, "Grid", "Threshold");
        registerFloat(gridThickness, "Grid", "Thickness");
    }
}
