#ifndef RESOURCEREPOSITORY_H
#define RESOURCEREPOSITORY_H
#include <unordered_map>

#include "RuntimeResource.h"

namespace Metal {
    struct ResourceRepository final {
        std::unordered_map<std::string, RuntimeResource *> resources{};
    };
} // Metal

#endif //RESOURCEREPOSITORY_H
