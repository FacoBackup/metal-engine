#ifndef DOCKREPOSITORY_H
#define DOCKREPOSITORY_H

#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "../../common/IRepository.h"
#include "../dto/DockDTO.h"

namespace Metal {
    struct DockRepository final : IRepository {
        std::shared_ptr<DockDTO> center = std::make_shared<DockDTO>(&DockSpace::VIEWPORT);
        std::vector<std::shared_ptr<DockDTO>> bottom;
        std::vector<std::shared_ptr<DockDTO>> left;
        std::vector<std::shared_ptr<DockDTO>> right;

        void registerFields() override {}

        const char *getTitle() const override {
            return "Docking";
        }

        const char *getIcon() const override {
            return "";
        }

        [[nodiscard]] nlohmann::json toJson() const override {
            nlohmann::json j;
            j["center"] = center->toJson();
            
            nlohmann::json leftJ = nlohmann::json::array();
            for (const auto& d : left) leftJ.push_back(d->toJson());
            j["left"] = leftJ;

            nlohmann::json rightJ = nlohmann::json::array();
            for (const auto& d : right) rightJ.push_back(d->toJson());
            j["right"] = rightJ;

            nlohmann::json bottomJ = nlohmann::json::array();
            for (const auto& d : bottom) bottomJ.push_back(d->toJson());
            j["bottom"] = bottomJ;

            return j;
        }

        void fromJson(const nlohmann::json &j) override {
            if (j.contains("center")) {
                center = DockDTO::fromJson(j.at("center"));
            }
            if (j.contains("left")) {
                left.clear();
                for (const auto& item : j.at("left")) {
                    left.push_back(DockDTO::fromJson(item));
                }
            }
            if (j.contains("right")) {
                right.clear();
                for (const auto& item : j.at("right")) {
                    right.push_back(DockDTO::fromJson(item));
                }
            }
            if (j.contains("bottom")) {
                bottom.clear();
                for (const auto& item : j.at("bottom")) {
                    bottom.push_back(DockDTO::fromJson(item));
                }
            }
        }
    };
}

#endif //DOCKREPOSITORY_H
