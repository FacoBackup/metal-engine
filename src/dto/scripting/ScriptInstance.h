#ifndef SCRIPTINSTANCE_H
#define SCRIPTINSTANCE_H
#include "NodeType.h"
#include "script-definitions.h"
#include "abstract/AbstractNode.h"

namespace Metal {
    struct Link;

    struct ScriptInstance final{
        const std::string id = NEW_ID;

        std::unordered_map<std::string, bool> selectedNodes;
        std::unordered_map<NodeId, std::shared_ptr<AbstractNode>> nodes;
        std::unordered_map<IOId, std::shared_ptr<Link>> links;

        explicit ScriptInstance();

        void addNode(NodeType type);

        void removeNode(const NodeId &nodeId);

        void addLink(IO source, IO target);

        void removeLink(IOId &id);
    };
} // Metal

#endif //SCRIPTINSTANCE_H
