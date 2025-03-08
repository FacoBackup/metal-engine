#ifndef ABSTRACTNODE_H
#define ABSTRACTNODE_H
#include "../script-definitions.h"
#include "glm/vec3.hpp"
#include "../IO.h"

namespace Metal {
    struct AbstractNode {
        int x, y, width, height;
        std::string name = "New Node";
        glm::vec3 highlightColor;
        NodeId id = NEW_ID;
        bool isDragging = false;
        std::vector<IO> io;

        explicit AbstractNode() : x(0), y(0), width(0), height(0), highlightColor() {
        }

        virtual ~AbstractNode() = default;


        virtual void execute() {
        }

        void initialize() {
            unsigned int offsetInput = 0;
            unsigned int offsetOutput = 0;
            if (isStartNode()) {
                io.push_back(IO{nullptr, false, IOType::IO_FLOW, 0, id});
                offsetOutput++;
            } else {
                io.push_back(IO{nullptr, false, IOType::IO_FLOW, 0, id});
                io.push_back(IO{nullptr, true, IOType::IO_FLOW, 0, id});
                offsetOutput++;
                offsetInput++;
            }
            for (auto ioLocal: getIO()) {
                if (ioLocal.isInput) {
                    ioLocal.offsetY += offsetInput;
                }else {
                    ioLocal.offsetY += offsetOutput;
                }
                io.push_back(ioLocal);
            }
        }

        virtual bool isStartNode() {
            return false;
        }

    protected:
        virtual std::vector<IO> getIO() {
            return {};
        }
    };
} // Metal

#endif //ABSTRACTNODE_H
