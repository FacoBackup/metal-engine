#ifndef ABSTRACTTYPE_H
#define ABSTRACTTYPE_H
#include "AbstractNode.h"
#include "../IO.h"

namespace Metal {
    template<typename T>
    struct AbstractType : AbstractNode {
        T value;

        explicit AbstractType();

        std::vector<IO> getIO() override {
            return {
                IO{true, getType(), 0, id},
                IO{false, getType(), 0, id},
            };
        }

        virtual IOType getType() {
            throw std::runtime_error("AbstractType::getType() called");
        }
    };

    template<typename T>
    AbstractType<T>::AbstractType() {
        height = MEDIUM_NODE_H;
        width = MEDIUM_NODE_W;
    }
}

#endif //ABSTRACTTYPE_H
