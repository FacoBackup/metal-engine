#ifndef IDENTIFIABLE_H
#define IDENTIFIABLE_H
#include "Util.h"

namespace Metal {
    class Identifiable {
    public:
        const std::string id = Util::uuidV4();
    };
}
#endif
