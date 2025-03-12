#ifndef SCRIPTINSTANCE_H
#define SCRIPTINSTANCE_H
#include "NodeType.h"
#include "script-definitions.h"
#include "abstract/AbstractDraggable.h"
#include "Link.h"
#include "abstract/AbstractVariable.h"

namespace Metal {
    struct ScriptInstance final {
        const std::string id = NEW_ID;

        std::unordered_map<std::string, bool> selectedNodes;
        std::unordered_map<NodeId, AbstractDraggable> nodes;
        std::unordered_map<IOId, Link> links;
        std::unordered_map<NodeId, AbstractVariable> variables;

        explicit ScriptInstance();

        void addNode(NodeType type);

        void removeNode(const NodeId &nodeId);

        void addLink(const IO &source, const IO &target);

        void removeLink(IOId &id);
    };
} // Metal

#endif //SCRIPTINSTANCE_H
