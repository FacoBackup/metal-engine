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

    void DockDefinition::registerFields() {
        registerSerializableOnlyField(&id, STRING, "id");
        registerSerializableOnlyField(&selectedOption, INT, "selectedOption");
        registerSerializableOnlyField(&internalId, STRING, "internalId");
        registerSerializableOnlyField(&splitDir, INT, "splitDir");
        registerSerializableOnlyField(&sizeX, FLOAT, "sizeX");
        registerSerializableOnlyField(&sizeY, FLOAT, "sizeY");
        registerSerializableOnlyField(&sizeRatioForNodeAtDir, FLOAT, "sizeRatioForNodeAtDir");

        auto dockSpacesToJson = [this] {
            std::vector<int> spaces;
            for (auto *ds: dockSpaces) {
                spaces.push_back(ds->index);
            }
            return spaces;
        };

        auto dockSpacesFromJson = [this](const nlohmann::json &j) {
            dockSpaces.clear();
            for (int index: j.get<std::vector<int> >()) {
                if (index == -1) {
                    dockSpaces.push_back(&DockSpace::VIEWPORT);
                } else {
                    dockSpaces.push_back(DockSpace::GetOption(index));
                }
            }
        };

        registerSerializableOnlyField(&dockSpaces, COMPOSITE, "dockSpaces")
                .setTransformer(dockSpacesToJson, dockSpacesFromJson);
    }
}
