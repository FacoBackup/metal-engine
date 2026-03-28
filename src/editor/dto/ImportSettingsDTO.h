#ifndef IMPORTSETTINGSDTO_H
#define IMPORTSETTINGSDTO_H

#include "../../common//Reflection.h"


namespace Metal {
    struct ImportSettingsDTO : Reflection {
        ImportSettingsDTO() = default;

        const char *getIcon() const override { return ""; }

        const char *getTitle() const override { return "Import Settings"; }
    };
}

#endif //IMPORTSETTINGSDTO_H
