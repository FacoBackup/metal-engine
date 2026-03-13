#ifndef METAL_ENGINE_DOCKDTO_H
#define METAL_ENGINE_DOCKDTO_H

#include <string>
#include <vector>
#include <imgui.h>
#include "../../../repository/dock/DockSpace.h"
#include "../util/Util.h"

namespace Metal {
    struct DockDTO {
        const std::string id = Util::uuidV4();
        ImGuiID nodeId{};
        int selectedOption;
        std::string internalId;
        ImGuiDir splitDir = ImGuiDir_Down;
        float sizeX{};
        float sizeY{};
        float sizeRatioForNodeAtDir{};
        DockDTO *outAtOppositeDir = nullptr;
        DockDTO *origin = nullptr;
        std::vector<DockSpace *> dockSpaces{};


        explicit DockDTO(DockSpace *description) : selectedOption(description->index),
                                                   internalId("##" + Util::uuidV4()) {
            dockSpaces.emplace_back(description);
        }

        explicit DockDTO(DockSpace *description, float sizeRatioForNodeAtDir) : selectedOption(description->index),
            internalId("##" + Util::uuidV4()),
            sizeRatioForNodeAtDir(sizeRatioForNodeAtDir) {
            dockSpaces.emplace_back(description);
        }
    };
} // Metal

#endif
