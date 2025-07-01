#ifndef METAL_ENGINE_DOCKDTO_H
#define METAL_ENGINE_DOCKDTO_H

#include <string>
#include <imgui.h>
#include "DockSpace.h"
#include "DockPosition.h"
#include "DockViewDTO.h"
#include "../../../common/Util.h"
#include "../../../common/serialization-definitions.h"

namespace Metal {
    struct DockDTO {
        std::string id = Util::uuidV4();
        ImGuiID nodeId{};
        std::string internalId;
        ImGuiDir splitDir = ImGuiDir_Down;
        float sizeX{};
        float sizeY{};
        float sizeRatioForNodeAtDir{};
        DockPosition direction = LEFT;
        std::vector<DockViewDTO> openTabs;

        DockDTO *outAtOppositeDir = nullptr;
        DockDTO *origin = nullptr;

        explicit DockDTO(const std::vector<unsigned int> &tabs) {
            internalId = "##" + Util::uuidV4();
            for (unsigned int tab: tabs) {
                openTabs.push_back({nullptr, tab});
            }
        }

        explicit DockDTO() = default;

        void addNewTab(unsigned int dockSpaceIndex) {
            openTabs.push_back({nullptr, dockSpaceIndex});
        }

        SERIALIZE_TEMPLATE(
            id,
            internalId,
            splitDir,
            sizeX,
            sizeY,
            sizeRatioForNodeAtDir,
            direction,
            openTabs
        )
    };
} // Metal

#endif
