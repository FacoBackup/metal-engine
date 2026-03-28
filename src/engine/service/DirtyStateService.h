#ifndef DIRTYSTATESERVICE_H
#define DIRTYSTATESERVICE_H

#include <common/IService.h>
#include <engine/enum/DirtyType.h>
#include <entt/entt.hpp>
#include <atomic>
#include <unordered_map>
#include <shared_mutex>

namespace Metal {
    /**
     * @brief A centralized service for managing "dirty" states across different engine modules.
     * 
     * This service provides a thread-safe registry for both engine-wide global flags and
     * entity-specific flags. It uses std::atomic for global flags and shared_mutex for
     * entity-specific maps, allowing concurrent reads and safe updates across different threads.
     * 
     * Typical usage involves:
     * - Producers: Marking a component or global state as dirty when it changes.
     * - Consumers: Checking for dirty states and consuming flags during synchronization phases.
     */
    class DirtyStateService final : public IService {
        /**
         * @brief Atomic bitmask for global engine states.
         */
        std::atomic<uint32_t> globalFlags{0};

        mutable std::shared_mutex entityMutex;
        /**
         * @brief Map of entity-specific dirty flag bitmasks.
         */
        std::unordered_map<entt::entity, uint32_t> entityFlags;

    public:
        /**
         * @brief Marks a global state as dirty.
         * @param type The DirtyType to mark.
         */
        void markDirty(DirtyType type);

        /**
         * @brief Checks if a global state is marked as dirty.
         * @param type The DirtyType to check.
         * @return True if the specified flags are set.
         */
        bool isDirty(DirtyType type) const;

        /**
         * @brief Atomically retrieves and clears specific global flags.
         * @param type The mask of flags to clear. Defaults to All.
         * @return The flags that were set before clearing.
         */
        DirtyType consumeDirtyFlags(DirtyType type = DirtyType::All);

        /**
         * @brief Marks a specific entity as dirty.
         * @param entity The entity handle.
         * @param type The DirtyType flags to set for this entity.
         */
        void markEntityDirty(entt::entity entity, DirtyType type);

        /**
         * @brief Checks if an entity is marked with specific dirty flags.
         * @param entity The entity handle.
         * @param type The DirtyType to check.
         * @return True if the entity is dirty with the specified flags.
         */
        bool isEntityDirty(entt::entity entity, DirtyType type) const;

        /**
         * @brief Retrieves and clears specific dirty flags for an entity.
         * @param entity The entity handle.
         * @param type The mask of flags to clear. Defaults to All.
         * @return The flags that were set for this entity before clearing.
         */
        DirtyType consumeEntityDirtyFlags(entt::entity entity, DirtyType type = DirtyType::All);

        /**
         * @brief Checks if any entity in the registry is marked with the specified dirty flags.
         * @param type The DirtyType mask to check across all entities.
         * @return True if at least one entity is dirty with matching flags.
         */
        bool hasAnyEntityDirty(DirtyType type) const;

        /**
         * @brief Retrieves all entities marked with specific dirty flags.
         * @param type The DirtyType mask to search for.
         * @param consume If true, clears matching flags from the retrieved entities.
         * @return A vector of entities that match the specified dirty criteria.
         */
        std::vector<entt::entity> getDirtyEntities(DirtyType type, bool consume = false);

        /**
         * @brief Clears all entity-specific flags.
         */
        void clearEntityFlags();
    };
}

#endif //DIRTYSTATESERVICE_H
