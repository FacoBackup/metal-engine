#include "DockRepository.h"

#include "editor/dto/DockSpace.h"

namespace Metal {
    void DockRepository::registerFields() {
        registerSerializableOnlyField(&center, COMPOSITE, "center").setTransformer(
            [this] {
                if (center == nullptr) return nlohmann::json(nullptr);
                return center->toJson();
            },
            [this](const nlohmann::json &j) {
                if (j.is_null()) {
                    center = nullptr;
                    return;
                }
                if (center == nullptr) center = std::make_shared<DockDefinition>();
                center->fromJson(j);
            });

        auto dockListToJson = [](std::vector<std::shared_ptr<DockDefinition> > &list) {
            nlohmann::json j = nlohmann::json::array();
            for (const auto &d: list) j.push_back(d->toJson());
            return j;
        };

        auto dockListFromJson = [](const nlohmann::json &j, std::vector<std::shared_ptr<DockDefinition> > &list) {
            list.clear();
            for (const auto &item: j) {
                auto d = std::make_shared<DockDefinition>();
                d->fromJson(item);
                list.push_back(d);
            }
        };

        registerSerializableOnlyField(&left, COMPOSITE, "left").setTransformer(
            [this, dockListToJson] { return dockListToJson(left); },
            [this, dockListFromJson](const nlohmann::json &j) { dockListFromJson(j, left); });
        registerSerializableOnlyField(&right, COMPOSITE, "right").setTransformer(
            [this, dockListToJson] { return dockListToJson(right); },
            [this, dockListFromJson](const nlohmann::json &j) { dockListFromJson(j, right); });
        registerSerializableOnlyField(&bottom, COMPOSITE, "bottom").setTransformer(
            [this, dockListToJson] { return dockListToJson(bottom); },
            [this, dockListFromJson](const nlohmann::json &j) { dockListFromJson(j, bottom); });
    }

    void DockRepository::onInitialize() {
        center = std::make_shared<DockDefinition>(&DockSpace::VIEWPORT);
    }
}
