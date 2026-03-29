#ifndef RGRESOURCEHANDLE_H
#define RGRESOURCEHANDLE_H

#include <cstdint>
#include <limits>
#include <string>

namespace Metal {
    /**
     * @class RGResourceHandle
     * @brief A type-safe wrapper around a uint32_t index for Render Graph resources.
     */
    class RGResourceHandle {
        uint32_t index;

    public:
        explicit RGResourceHandle(uint32_t index) : index(index) {
        }

        RGResourceHandle() : index(std::numeric_limits<uint32_t>::max()) {
        }

        [[nodiscard]] uint32_t getIndex() const { return index; }
        [[nodiscard]] bool isValid() const { return index != std::numeric_limits<uint32_t>::max(); }

        bool operator==(const RGResourceHandle &other) const { return index == other.index; }
        bool operator!=(const RGResourceHandle &other) const { return index != other.index; }
        bool operator<(const RGResourceHandle &other) const { return index < other.index; }

        explicit operator std::string() const {
            return isValid() ? "__RG_HANDLE_" + std::to_string(index) : "__RG_INVALID_HANDLE";
        }
    };
}

#endif // RGRESOURCEHANDLE_H
