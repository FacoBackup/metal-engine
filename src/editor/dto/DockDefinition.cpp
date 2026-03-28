#include "DockDefinition.h"

#include "DockSpace.h"
#include "common/FieldType.h"

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
        registerSerializableOnlyField<STRING>(&id).setName("id");
        registerSerializableOnlyField<INT>(&selectedOption).setName("selectedOption");
        registerSerializableOnlyField<STRING>(&internalId).setName("internalId");
        registerSerializableOnlyField<INT>((int *) &splitDir).setName("splitDir");
        registerSerializableOnlyField<FLOAT>(&sizeX).setName("sizeX");
        registerSerializableOnlyField<FLOAT>(&sizeY).setName("sizeY");
        registerSerializableOnlyField<FLOAT>(&sizeRatioForNodeAtDir).setName("sizeRatioForNodeAtDir");

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

        registerGenericField(dockSpacesToJson, dockSpacesFromJson).setName("dockSpaces");
    }
}
