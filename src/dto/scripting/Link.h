#ifndef ABSTRACTLINK_H
#define ABSTRACTLINK_H
#include <utility>

#include "IO.h"
#include "script-definitions.h"

namespace Metal {
    struct Link {
        const IOId id = NEW_ID;
        std::string source;
        std::string target;
        std::string sourceNodeId;
        std::string targetNodeId;


        explicit
        Link(std::string source, std::string target, std::string sourceNodeId,
             std::string targetNodeId) : source(std::move(source)), target(std::move(target)),
                                         sourceNodeId(std::move(sourceNodeId)), targetNodeId(std::move(targetNodeId)) {
        }
    };
} // Metal

#endif //ABSTRACTLINK_H
