#include "DockDefinition.h"

#include "DockSpace.h"

namespace Metal {
    bool DockDefinition::isCenter() const {
        return selectedOption == DockSpace::VIEWPORT.index;
    }

    DockDefinition::DockDefinition(DockSpace *description) : selectedOption(description->index),
                                                             internalId("##" + Util::uuidV4()) {
        dockSpaces.emplace_back(description);
    }

    DockDefinition::DockDefinition(DockSpace *description, float sizeRatioForNodeAtDir) : selectedOption(
                                                                                              description->index),
                                                                                          internalId("##" + Util::uuidV4()),
                                                                                          sizeRatioForNodeAtDir(sizeRatioForNodeAtDir) {
        dockSpaces.emplace_back(description);
    }

    nlohmann::json DockDefinition::toJson() const {
        nlohmann::json j;
        j["id"] = id;
        j["selectedOption"] = selectedOption;
        j["internalId"] = internalId;
        j["splitDir"] = splitDir;
        j["sizeX"] = sizeX;
        j["sizeY"] = sizeY;
        j["sizeRatioForNodeAtDir"] = sizeRatioForNodeAtDir;
        std::vector<int> spaces;
        for (auto *ds: dockSpaces) {
            spaces.push_back(ds->index);
        }
        j["dockSpaces"] = spaces;
        return j;
    }

    void DockDefinition::fromJson(const nlohmann::json &j) {
        id = j.at("id").get<std::string>();
        selectedOption = j.at("selectedOption").get<int>();
        internalId = j.at("internalId").get<std::string>();
        splitDir = j.at("splitDir").get<ImGuiDir>();
        sizeX = j.at("sizeX").get<float>();
        sizeY = j.at("sizeY").get<float>();
        sizeRatioForNodeAtDir = j.at("sizeRatioForNodeAtDir").get<float>();
        dockSpaces.clear();
        for (int index: j.at("dockSpaces").get<std::vector<int> >()) {
            if (index == -1) {
                dockSpaces.push_back(&DockSpace::VIEWPORT);
            } else {
                dockSpaces.push_back(DockSpace::GetOption(index));
            }
        }
    }
}
