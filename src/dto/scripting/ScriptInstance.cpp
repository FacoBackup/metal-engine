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
        switch (type) {
            case AND: {
                AndNode node;
                node.initialize();
                nodes.emplace(node.id, node);
                break;
            }
            case START: {
                StartNode node;
                node.initialize();
                nodes.emplace(node.id, node);
                break;
            }
            default:
                break;
        }
    }

    void ScriptInstance::removeNode(const NodeId &nodeId) {
        if (nodes.contains(nodeId)) {
            std::vector<Link * > toRemove{};
            for (auto &link: links) {
                if (link.second.sourceNodeId == nodeId || link.second.targetNodeId == nodeId) {
                    toRemove.emplace_back(&link.second);
                }
            }
            for (auto &remove: toRemove) {
                links.erase(remove->id);
            }
            nodes.erase(nodeId);
        }
    }

    void ScriptInstance::addLink(const IO &source, const IO &target) {
        if (source.type == target.type) {
            Link link{source.id, target.id, source.nodeId, target.nodeId};
            links.emplace(link.id, link);
        }
    }

    void ScriptInstance::removeLink(IOId &id) {
        links.erase(id);
    }
} // Metal
