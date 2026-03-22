#ifndef IMPORTSETTINGSDTO_H
#define IMPORTSETTINGSDTO_H

#include "../../common//Inspectable.h"


namespace Metal {
    struct ImportSettingsDTO : Inspectable {
        ImportSettingsDTO() = default;

        const char *getIcon() const override { return ""; }

        const char *getTitle() const override { return "Import Settings"; }
    };
}

#endif //IMPORTSETTINGSDTO_H
