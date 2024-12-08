#ifndef IDENTIFIABLE_H
#define IDENTIFIABLE_H
#include "../util/Util.h"

namespace Metal {
    class Identifiable {
    public:
        virtual ~Identifiable() = default;

        std::string id = Util::uuidV4();
    };
}
#endif
