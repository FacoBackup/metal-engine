#ifndef METAL_ENGINE_DOCKDTO_H
#define METAL_ENGINE_DOCKDTO_H

#include <string>
#include <vector>
#include <imgui.h>
#include <memory>
#include <nlohmann/json.hpp>

#include "common/Reflection.h"
#include "common/Util.h"

namespace Metal {
    class DockSpace;

    struct DockDefinition : Reflection {
        std::string id = Util::uuidV4();
        ImGuiID nodeId{};
        int selectedOption;
        std::string internalId;
        ImGuiDir splitDir = ImGuiDir_Down;
        float sizeX{};
        float sizeY{};
        float sizeRatioForNodeAtDir{};
        std::shared_ptr<DockDefinition> outAtOppositeDir = nullptr;
        std::shared_ptr<DockDefinition> origin = nullptr;
        std::vector<DockSpace *> dockSpaces{};

        explicit DockDefinition(DockSpace *description);

        explicit DockDefinition() : selectedOption(0) {
        }

        explicit DockDefinition(DockSpace *description, float sizeRatioForNodeAtDir);

        void registerFields() override;

        [[nodiscard]] bool isCenter() const;
    };
} // Metal

#endif
