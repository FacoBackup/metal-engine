#include <engine/service/DirtyStateService.h>

#include "common/LoggerUtil.h"

namespace Metal {

    void DirtyStateService::markDirty(DirtyType type) {
        globalFlags.fetch_or(static_cast<uint32_t>(type), std::memory_order_relaxed);
    }

    bool DirtyStateService::isDirty(DirtyType type) const {
        return (globalFlags.load(std::memory_order_relaxed) & static_cast<uint32_t>(type)) != 0;
    }

    DirtyType DirtyStateService::consumeDirtyFlags(DirtyType type) {
        uint32_t mask = static_cast<uint32_t>(type);
        uint32_t expected = globalFlags.load(std::memory_order_relaxed);
        uint32_t desired;
        do {
            desired = expected & ~mask;
        } while (!globalFlags.compare_exchange_weak(expected, desired, std::memory_order_relaxed));
        return static_cast<DirtyType>(expected & mask);
    }

    void DirtyStateService::markEntityDirty(entt::entity entity, DirtyType type) {
        std::unique_lock lock(entityMutex);
        entityFlags[entity] |= static_cast<uint32_t>(type);
    }

    bool DirtyStateService::isEntityDirty(entt::entity entity, DirtyType type) const {
        std::shared_lock lock(entityMutex);
        auto it = entityFlags.find(entity);
        if (it != entityFlags.end()) {
            return (it->second & static_cast<uint32_t>(type)) != 0;
        }
        return false;
    }

    DirtyType DirtyStateService::consumeEntityDirtyFlags(entt::entity entity, DirtyType type) {
        std::unique_lock lock(entityMutex);
        auto it = entityFlags.find(entity);
        if (it != entityFlags.end()) {
            uint32_t mask = static_cast<uint32_t>(type);
            uint32_t flags = it->second & mask;
            it->second &= ~mask;
            if (it->second == 0) {
                entityFlags.erase(it);
            }
            return static_cast<DirtyType>(flags);
        }
        return DirtyType::None;
    }

    bool DirtyStateService::hasAnyEntityDirty(DirtyType type) const {
        std::shared_lock lock(entityMutex);
        uint32_t mask = static_cast<uint32_t>(type);
        for (const auto& [entity, flags] : entityFlags) {
            if ((flags & mask) != 0) return true;
        }
        return false;
    }

    std::vector<entt::entity> DirtyStateService::getDirtyEntities(DirtyType type, bool consume) {
        std::unique_lock lock(entityMutex);
        std::vector<entt::entity> result;
        uint32_t mask = static_cast<uint32_t>(type);
        
        for (auto it = entityFlags.begin(); it != entityFlags.end(); ) {
            if ((it->second & mask) != 0) {
                result.push_back(it->first);
                if (consume) {
                    it->second &= ~mask;
                    if (it->second == 0) {
                        it = entityFlags.erase(it);
                        continue;
                    }
                }
            }
            ++it;
        }
        return result;
    }

    void DirtyStateService::clearEntityFlags() {
        std::unique_lock lock(entityMutex);
        entityFlags.clear();
    }

} // Metal
