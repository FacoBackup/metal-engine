#ifndef INSPECTEDFIELD_H
#define INSPECTEDFIELD_H

#include <memory>
#include <vector>
#include <optional>
#include "InspectableMember.h"

namespace Metal {
    struct FileExtensionInfo;

    template<typename T>
    struct InspectedField final : InspectableMember {
        T *field;
        T previousValue;

        std::optional<int> max;
        std::optional<int> min;
        std::optional<float> maxF;
        std::optional<float> minF;
        std::optional<float> incrementF;
        bool disabled = false;
        std::vector<const FileExtensionInfo *> supportedFileTypes{};

        explicit InspectedField(T *field) : field(field), previousValue(*field) {
        }

        ~InspectedField() override = default;

        void *getGenericPointer() override {
            return field;
        }
    };
}
#endif
