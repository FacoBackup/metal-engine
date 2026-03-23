#ifndef METAL_ENGINE_DOCKDTO_H
#define METAL_ENGINE_DOCKDTO_H

#include <string>
#include <vector>
#include <imgui.h>
#include <memory>
#include <nlohmann/json.hpp>
#include "DockSpace.h"
#include "common/Util.h"

namespace Metal {
    struct DockDTO {
        bool isCenter = false;
        std::string id = Util::uuidV4();
        ImGuiID nodeId{};
        int selectedOption;
        std::string internalId;
        ImGuiDir splitDir = ImGuiDir_Down;
        float sizeX{};
        float sizeY{};
        float sizeRatioForNodeAtDir{};
        std::shared_ptr<DockDTO> outAtOppositeDir = nullptr;
        std::shared_ptr<DockDTO> origin = nullptr;
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

        [[nodiscard]] nlohmann::json toJson() const {
            nlohmann::json j;
            j["isCenter"] = isCenter;
            j["id"] = id;
            j["selectedOption"] = selectedOption;
            j["internalId"] = internalId;
            j["splitDir"] = splitDir;
            j["sizeX"] = sizeX;
            j["sizeY"] = sizeY;
            j["sizeRatioForNodeAtDir"] = sizeRatioForNodeAtDir;
            std::vector<int> spaces;
            for (auto *ds : dockSpaces) {
                spaces.push_back(ds->index);
            }
            j["dockSpaces"] = spaces;
            return j;
        }

        static std::shared_ptr<DockDTO> fromJson(const nlohmann::json &j) {
            auto d = std::make_shared<DockDTO>(&DockSpace::VIEWPORT);
            d->isCenter = j.at("isCenter").get<bool>();
            d->id = j.at("id").get<std::string>();
            d->selectedOption = j.at("selectedOption").get<int>();
            d->internalId = j.at("internalId").get<std::string>();
            d->splitDir = j.at("splitDir").get<ImGuiDir>();
            d->sizeX = j.at("sizeX").get<float>();
            d->sizeY = j.at("sizeY").get<float>();
            d->sizeRatioForNodeAtDir = j.at("sizeRatioForNodeAtDir").get<float>();
            d->dockSpaces.clear();
            for (int index : j.at("dockSpaces").get<std::vector<int>>()) {
                if (index == -1) {
                    d->dockSpaces.push_back(&DockSpace::VIEWPORT);
                } else {
                    d->dockSpaces.push_back(DockSpace::GetOption(index));
                }
            }
            return d;
        }
    };
} // Metal

#endif
