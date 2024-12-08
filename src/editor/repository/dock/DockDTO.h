#ifndef METAL_ENGINE_DOCKDTO_H
#define METAL_ENGINE_DOCKDTO_H

#include <string>
#include <imgui.h>
#include "DockSpace.h"
#include "DockPosition.h"
#include "../../../common/util/Util.h"

namespace Metal {
    struct DockDTO {
        const std::string id = Util::uuidV4();
        ImGuiID nodeId{};
        int selectedOption;
        const char * internalId = nullptr;
        ImGuiDir splitDir = ImGuiDir_Down;
        float sizeX{};
        float sizeY{};
        float sizeRatioForNodeAtDir{};
        DockDTO *outAtOppositeDir = nullptr;
        DockDTO *origin = nullptr;
        DockSpace *description;
        DockPosition direction = LEFT;

        explicit DockDTO(DockSpace *description) : selectedOption(description->index),
                                                   description(description) {
            internalId = ("##" + Util::uuidV4()).c_str();
        }
    };
} // Metal

#endif
