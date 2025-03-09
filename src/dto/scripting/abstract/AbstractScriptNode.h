#ifndef ABSTRACTNODE_H
#define ABSTRACTNODE_H
#include <imgui.h>
#include <vector>
#include <unordered_map>
#include <string>
#include "../script-definitions.h"
#include "glm/vec3.hpp"
#include "../IO.h"
#include "AbstractDraggable.h"

namespace Metal {
    struct AbstractScriptNode : AbstractDraggable {
        glm::vec3 highlightColor{};

        virtual void execute() {
        }

        virtual bool isStartNode() {
            return false;
        }

    protected:
        std::vector<IO> getIOInternal() override {
            std::vector<IO> tempIoList;

            unsigned int offsetInput = 0;
            unsigned int offsetOutput = 0;
            if (isStartNode()) {
                tempIoList.emplace_back(nullptr, false, IOType::IO_FLOW, 0, id);
                offsetOutput++;
            } else {
                tempIoList.emplace_back(nullptr, false, IOType::IO_FLOW, 0, id);
                tempIoList.emplace_back(nullptr, true, IOType::IO_FLOW, 0, id);
                offsetOutput++;
                offsetInput++;
            }

            for (auto ioLocal: getIO()) {
                if (ioLocal.isInput) {
                    ioLocal.offsetY += offsetInput;
                } else {
                    ioLocal.offsetY += offsetOutput;
                }
                tempIoList.push_back(ioLocal);
            }

            return tempIoList;
        }

        virtual std::vector<IO> getIO() {
            return {};
        }
    };
} // Metal

#endif //ABSTRACTNODE_H
