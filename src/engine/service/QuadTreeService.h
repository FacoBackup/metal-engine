#ifndef QUADTREESERVICE_H
#define QUADTREESERVICE_H

#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <common/IService.h>
#include <common/IInit.h>
#include <common/IAsyncSync.h>
#include <mutex>
#include <atomic>

namespace Metal {
    struct TransformComponent;
    struct WorldRepository;
    struct EngineRepository;
    struct CameraRepository;
    class DirtyStateService;

    struct QuadBounds {
        float x, z, size;

        bool contains(float px, float pz) const {
            return px >= x && px <= x + size && pz >= z && pz <= z + size;
        }

        bool intersects(const QuadBounds &other) const {
            return !(other.x > x + size || other.x + other.size < x || other.z > z + size || other.z + other.size < z);
        }
    };

    class QuadTreeNode {
    public:
        QuadBounds bounds;
        int level;
        std::vector<entt::entity> entities;
        std::unique_ptr<QuadTreeNode> children[4];
        bool isLeaf = true;

        explicit QuadTreeNode(QuadBounds b, int l) : bounds(b), level(l) {}

        void split();
        void insert(entt::entity entity, float x, float z, int maxLevel);
        void clear();
    };

    class QuadTreeService final : public IService, public IInit, public IAsyncSync {
        friend class TerrainGBufferPass;
        WorldRepository *worldRepository = nullptr;
        EngineRepository *engineRepository = nullptr;
        CameraRepository *cameraRepository = nullptr;
        DirtyStateService *dirtyStateService = nullptr;

        std::unique_ptr<QuadTreeNode> root;
        int maxLevel = 8;
        float worldSize = 10000.0f;

        std::vector<entt::entity> visibleEntities;
        std::mutex resultsMutex;

    public:
        std::vector<Dependency> getDependencies() override;

        void onInitialize() override;

        std::string getSyncThreadId() const override { return "QuadTreeUpdate"; }

        float getSyncInterval() const override { return 1.0f; }

        void onAsyncSync() override;

        ~QuadTreeService() override;

        void update();

        void queryVisible(std::vector<entt::entity> &outEntities);

        int getLODAt(float x, float z);

        void rebuild();
    };
}

#endif //QUADTREESERVICE_H
