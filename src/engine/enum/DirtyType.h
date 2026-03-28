#ifndef DIRTYTYPE_H
#define DIRTYTYPE_H

#include <cstdint>

namespace Metal {
    enum class DirtyType : uint32_t {
        None = 0,
        // Global Flags
        Camera = 1 << 0,
        GlobalSettings = 1 << 1,
        Environment = 1 << 2,
        
        // Entity Flags
        Transform = 1 << 3,
        Material = 1 << 4,
        Mesh = 1 << 5,
        
        // Special Flags
        BVH = 1 << 6,
        
        All = 0xFFFFFFFF
    };

    inline DirtyType operator|(DirtyType a, DirtyType b) {
        return static_cast<DirtyType>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline DirtyType operator&(DirtyType a, DirtyType b) {
        return static_cast<DirtyType>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    inline DirtyType operator~(DirtyType a) {
        return static_cast<DirtyType>(~static_cast<uint32_t>(a));
    }
}

#endif //DIRTYTYPE_H
