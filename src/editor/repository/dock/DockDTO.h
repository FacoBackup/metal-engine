#ifndef METAL_ENGINE_DOCKDTO_H
#define METAL_ENGINE_DOCKDTO_H

#include <string>
#include <imgui.h>
#include "../../../common/Util.h"
#include "DockSpace.h"
#include "DockPosition.h"

namespace Metal {
    struct DockDTO {
        const std::string id = Util::uuidV4();
        ImGuiID nodeId{};
        int selectedOption;
        const char * internalId;
        ImGuiDir splitDir = ImGuiDir_Down;
        float sizeX{};
        float sizeY{};
        float sizeRatioForNodeAtDir{};
        DockDTO *outAtOppositeDir = nullptr;
        DockDTO *origin = nullptr;
        DockSpace *description;
        DockPosition direction = LEFT;

        explicit DockDTO(DockSpace *description) : selectedOption(description->index), internalId(("##" + Util::uuidV4()).c_str()),
                                                   description(description) {
        }
    };
} // Metal

#endif
