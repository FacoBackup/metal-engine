#include "ScriptInstance.h"

#include "Link.h"
#include "impl/nodes/AndNode.h"
#include "impl/nodes/StartNode.h"
#include "impl/types/BooleanType.h"

namespace Metal {
    ScriptInstance::ScriptInstance() {
        addNode(START);
    }

    void ScriptInstance::addNode(NodeType type) {
        std::shared_ptr<AbstractNode> node = nullptr;
        switch (type) {
            case AND:
                node = std::make_shared<AndNode>();
                break;
            case NodeType::BOOL:
                node = std::make_shared<BooleanType>();
                break;
            case START:
                node = std::make_shared<StartNode>();
                break;
            default:
                return;
        }
        node->initialize();
        nodes.emplace(node->id, node);
    }

    void ScriptInstance::removeNode(const NodeId &nodeId) {
        if (nodes.contains(nodeId)) {
            std::vector<std::shared_ptr<Link> > toRemove{};
            for (auto &link: links) {
                if (link.second->source.nodeId == nodeId || link.second->target.nodeId == nodeId) {
                    toRemove.emplace_back(link.second);
                }
            }
            for (auto &remove: toRemove) {
                links.erase(remove->id);
            }
            nodes.erase(nodeId);
        }
    }

    void ScriptInstance::addLink(IO source, IO target) {
        if (source.type == target.type) {
            auto link = std::make_shared<Link>(source, target);
            links.emplace(link->id, link);
        }
    }

    void ScriptInstance::removeLink(IOId &id) {
        links.erase(id);
    }
} // Metal
