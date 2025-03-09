#ifndef LINKPANEL_H
#define LINKPANEL_H
#include "../../../abstract/AbstractPanel.h"

namespace Metal {
    struct ScriptInstance;
    struct Link;

    class LinkPanel final : public AbstractPanel {
        Link *link = nullptr;
        ScriptInstance *scriptInstance = nullptr;

    public:

        void setLink(Link *link) {
            this->link = link;
        }

        void setScriptInstance(ScriptInstance *scriptInstance) {
            this->scriptInstance = scriptInstance;
        }

        void onSync() override;
    };
} // Metal

#endif //LINKPANEL_H
