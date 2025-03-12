#ifndef ABSTRACTTYPENODE_H
#define ABSTRACTTYPENODE_H

#include "AbstractDraggable.h"
#include "../IO.h"

namespace Metal {
    struct AbstractTypeNode : AbstractDraggable {
        std::string key;

        explicit AbstractTypeNode(std::string key) : key(std::move(key)) {}

        std::vector<IO> getIOInternal() override {
            return {
                IO{"Setter", true, getType(), 0, id},
                IO{"Getter", false, getType(), 0, id},
            };
        }

        virtual IOType getType() {
            throw std::runtime_error("AbstractType::getType() called");
        }
    };
}

#endif //ABSTRACTTYPENODE_H
