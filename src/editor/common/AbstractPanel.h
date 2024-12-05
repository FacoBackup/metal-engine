#ifndef METAL_ENGINE_ABSTRACTPANEL_H
#define METAL_ENGINE_ABSTRACTPANEL_H

#include "IPanel.h"
#include "imgui.h"
#include "../../common/Util.h"

namespace Metal {
    class ApplicationContext;

    class AbstractPanel : public IPanel {
    protected:
        ApplicationContext *context = nullptr;
        std::vector<IPanel *> children;
        const std::string id = "##" + Util::uuidV4();

        void onSyncChildren() const;

    public:
        std::vector<IPanel *> &getChildren();

        void appendChild(AbstractPanel *panel);

        void removeAllChildren() override;

        void setContext(ApplicationContext *context);

        virtual void onRemove() {
        }
    };
}
#endif
