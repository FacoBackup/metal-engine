#ifndef ABSTRACTDRAGGABLE_H
#define ABSTRACTDRAGGABLE_H
#include <imgui.h>
#include <vector>
#include <unordered_map>
#include <string>
#include "../script-definitions.h"
#include "glm/vec3.hpp"
#include "../IO.h"

namespace Metal {
    struct AbstractDraggable {
        NodeId id = NEW_ID;
        int x, y, width, height;
        std::string name = "New Node";
        bool isDragging = false;
        std::vector<std::string> ioList{};
        std::unordered_map<std::string, IO> ioMap{};
        ImVec2 nodeWindowPos{};

        explicit AbstractDraggable() : x(0), y(0), width(0), height(0) {
        }

        virtual ~AbstractDraggable() = default;

        void initialize() {
            std::vector<IO> tempIoList = getIOInternal();
            for (auto &ioLocal: tempIoList) {
                std::string key = ioLocal.id;
                ioMap.emplace(key, ioLocal);
                ioList.push_back(key);
            }
        }

    protected:
        virtual std::vector<IO> getIOInternal() {
            return {};
        }
    };
} // Metal

#endif //ABSTRACTNODE_H
