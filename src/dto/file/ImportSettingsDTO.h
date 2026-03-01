#ifndef IMPORTSETTINGSDTO_H
#define IMPORTSETTINGSDTO_H

#include "../../common/inspection/Inspectable.h"
#include <memory>

namespace Metal {
    struct ImportSettingsDTO : Inspectable {
        ImportSettingsDTO() = default;

        const char *getIcon() override { return ""; }

        const char *getTitle() override { return "Import Settings"; }
    };
}

#endif //IMPORTSETTINGSDTO_H
