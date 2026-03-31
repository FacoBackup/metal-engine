#include "DockRepository.h"

#include "editor/dto/DockSpace.h"

namespace Metal {
    void DockRepository::clear() {
        center = nullptr;
        left.clear();
        right.clear();
        bottom.clear();
    }

    void DockRepository::registerFields() {
        registerGenericField(
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
            }).setName("center");

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

        registerGenericField(
            [this, dockListToJson] { return dockListToJson(left); },
            [this, dockListFromJson](const nlohmann::json &j) { dockListFromJson(j, left); }).setName("left");
        registerGenericField(
            [this, dockListToJson] { return dockListToJson(right); },
            [this, dockListFromJson](const nlohmann::json &j) { dockListFromJson(j, right); }).setName("right");
        registerGenericField(
            [this, dockListToJson] { return dockListToJson(bottom); },
            [this, dockListFromJson](const nlohmann::json &j) { dockListFromJson(j, bottom); }).setName("bottom");
    }

    void DockRepository::onInitialize() {
        center = std::make_shared<DockDefinition>(&DockSpace::VIEWPORT);
    }
}
