#include "DockRepository.h"

#include "editor/dto/DockSpace.h"

namespace Metal {
    void DockRepository::onInitialize() {
        center = std::make_shared<DockDefinition>(&DockSpace::VIEWPORT);
    }

    nlohmann::json DockRepository::toJson() const {
        nlohmann::json j;
        j["center"] = center->toJson();

        nlohmann::json leftJ = nlohmann::json::array();
        for (const auto &d: left) leftJ.push_back(d->toJson());
        j["left"] = leftJ;

        nlohmann::json rightJ = nlohmann::json::array();
        for (const auto &d: right) rightJ.push_back(d->toJson());
        j["right"] = rightJ;

        nlohmann::json bottomJ = nlohmann::json::array();
        for (const auto &d: bottom) bottomJ.push_back(d->toJson());
        j["bottom"] = bottomJ;

        return j;
    }

    void DockRepository::fromJson(const nlohmann::json &j) {
        if (j.contains("center")) {
            center = std::make_shared<DockDefinition>();
            center->fromJson(j.at("center"));
        }
        if (j.contains("left")) {
            left.clear();
            for (const auto &item: j.at("left")) {
                std::shared_ptr<DockDefinition> d = std::make_shared<DockDefinition>();
                d->fromJson(item);
                left.push_back(d);
            }
        }
        if (j.contains("right")) {
            right.clear();
            for (const auto &item: j.at("right")) {
                std::shared_ptr<DockDefinition> d = std::make_shared<DockDefinition>();
                d->fromJson(item);
                right.push_back(d);
            }
        }
        if (j.contains("bottom")) {
            bottom.clear();
            for (const auto &item: j.at("bottom")) {
                std::shared_ptr<DockDefinition> d = std::make_shared<DockDefinition>();
                d->fromJson(item);
                bottom.push_back(d);
            }
        }
    }
}
