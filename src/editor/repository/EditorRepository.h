#ifndef EDITORREPOSITORY_H
#define EDITORREPOSITORY_H
#include <imgui.h>
#include <ImGuizmo.h>
#include <vector>
#include <unordered_map>
#include <glm/vec3.hpp>

#include "../dto/ShortcutDTO.h"
#include "../enum/engine-definitions.h"
#include "../enum/ShadingMode.h"
#include "../../common/IRepository.h"

namespace Metal {
    struct TransformComponent;
    struct FSEntry;
    struct ImportSettingsDTO;

    struct EditorRepository final : IRepository {
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
        entt::entity mainSelection = EMPTY_ENTITY;
        std::unordered_map<entt::entity, bool> selected{};
        std::vector<entt::entity> copied{};
        std::string focusedWindowName{};
        std::vector<ShortcutDTO> focusedShortcuts{};
        ShadingMode shadingMode = LIT;


        std::vector<std::string> pendingImports;
        std::unordered_map<std::string, std::shared_ptr<ImportSettingsDTO> > importSettingsMap;
        std::string selectedFileForSettings;
        FSEntry *targetImportDirectory = nullptr;

        void registerFields() override;

        const char *getTitle() override;

        const char *getIcon() override;

        nlohmann::json toJson() const override;

        void fromJson(const nlohmann::json &j) override;
    };
} // Metal

#endif
