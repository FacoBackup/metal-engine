#include "QuadTreeService.h"
#include <engine/service/DirtyStateService.h>
#include "../dto/TransformComponent.h"
#include "../repository/WorldRepository.h"
#include "../repository/EngineRepository.h"
#include "../repository/CameraRepository.h"
#include <algorithm>
#include <chrono>

namespace Metal {

    void QuadTreeNode::split() {
        float subSize = bounds.size / 2.0f;
        children[0] = std::make_unique<QuadTreeNode>(QuadBounds{bounds.x, bounds.z, subSize}, level + 1);
        children[1] = std::make_unique<QuadTreeNode>(QuadBounds{bounds.x + subSize, bounds.z, subSize}, level + 1);
        children[2] = std::make_unique<QuadTreeNode>(QuadBounds{bounds.x, bounds.z + subSize, subSize}, level + 1);
        children[3] = std::make_unique<QuadTreeNode>(QuadBounds{bounds.x + subSize, bounds.z + subSize, subSize}, level + 1);
        isLeaf = false;
    }

    void QuadTreeNode::insert(entt::entity entity, float x, float z, int maxLevel) {
        if (level < maxLevel) {
            if (isLeaf) split();
            for (auto &child : children) {
                if (child->bounds.contains(x, z)) {
                    child->insert(entity, x, z, maxLevel);
                    return;
                }
            }
        }
        entities.push_back(entity);
    }

    void QuadTreeNode::clear() {
        entities.clear();
        if (!isLeaf) {
            for (auto &child : children) {
                child->clear();
            }
        }
    }

    std::vector<Dependency> QuadTreeService::getDependencies() {
        return {
            {"WorldRepository", &worldRepository},
            {"EngineRepository", &engineRepository},
            {"CameraRepository", &cameraRepository},
            {"DirtyStateService", &dirtyStateService}
        };
    }

    void QuadTreeService::onInitialize() {
        rebuild();
    }

    void QuadTreeService::onAsyncSync() {
        if (dirtyStateService->isDirty(DirtyType::QuadTree) || dirtyStateService->isDirty(DirtyType::Camera)) {
            update();
            dirtyStateService->consumeDirtyFlags(DirtyType::QuadTree);
        }
    }

    QuadTreeService::~QuadTreeService() {
    }

    void QuadTreeService::rebuild() {
        // We could get world size from EngineRepository if available
        root = std::make_unique<QuadTreeNode>(QuadBounds{-worldSize / 2.0f, -worldSize / 2.0f, worldSize}, 0);
    }

    void QuadTreeService::update() {
        root->clear();
        auto view = worldRepository->registry.view<TransformComponent>();
        for (auto entity : view) {
            auto &transform = view.get<TransformComponent>(entity);
            root->insert(entity, transform.translation.x, transform.translation.z, maxLevel);
        }

        std::vector<entt::entity> newVisible;
        auto traverse = [&](auto &self, QuadTreeNode *node) -> void {
            if (!cameraRepository->isAABBInsideFrustum(
                    glm::vec3(node->bounds.x, -1000.0f, node->bounds.z),
                    glm::vec3(node->bounds.x + node->bounds.size, 1000.0f, node->bounds.z + node->bounds.size))) {
                return;
            }

            newVisible.insert(newVisible.end(), node->entities.begin(), node->entities.end());
            if (!node->isLeaf) {
                for (auto &child : node->children) {
                    self(self, child.get());
                }
            }
        };
        traverse(traverse, root.get());

        std::lock_guard<std::mutex> lock(resultsMutex);
        visibleEntities = std::move(newVisible);
    }

    void QuadTreeService::queryVisible(std::vector<entt::entity> &outEntities) {
        std::lock_guard<std::mutex> lock(resultsMutex);
        outEntities.insert(outEntities.end(), visibleEntities.begin(), visibleEntities.end());
    }

    int QuadTreeService::getLODAt(float x, float z) {
        QuadTreeNode *current = root.get();
        while (current && !current->isLeaf) {
            bool found = false;
            for (auto &child : current->children) {
                if (child->bounds.contains(x, z)) {
                    current = child.get();
                    found = true;
                    break;
                }
            }
            if (!found) break;
        }
        return current ? current->level : 0;
    }
}
