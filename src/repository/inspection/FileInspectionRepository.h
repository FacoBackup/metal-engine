#ifndef FILEINSPECTIONREPOSITORY_H
#define FILEINSPECTIONREPOSITORY_H
#include "../../common/inspection/Inspectable.h"

namespace Metal {
    struct FileInspectionRepository final : Inspectable {
        std::string materialId;

        void registerFields() override;

        const char *getIcon() override;

        const char *getTitle() override;
    };
} // Metal

#endif //FILEINSPECTIONREPOSITORY_H
