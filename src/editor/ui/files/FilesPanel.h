#ifndef FILESPANEL_H
#define FILESPANEL_H
#include "../docks/AbstractDockPanel.h"

namespace Metal {
    class FilesPanel final : public AbstractDockPanel {
    public:
        void onSync() override;
    };
} // Metal

#endif //FILESPANEL_H
