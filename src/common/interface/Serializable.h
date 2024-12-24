#ifndef J_SERIALIZABLE_H
#define J_SERIALIZABLE_H

#include <string>

namespace Metal {
    struct Serializable {
        virtual ~Serializable() = default;

        virtual std::string serialize() { return ""; }

        virtual void deserialize(const std::string &json) {
        }
    };
}
#endif
