#ifndef METAL_ENGINE_ABSTRACTPANEL_H
#define METAL_ENGINE_ABSTRACTPANEL_H

#include "IPanel.h"
#include "../../common/runtime/ApplicationContext.h"

namespace Metal {
    class AbstractPanel : public IPanel {
    protected:
        ApplicationContext *context = nullptr;
        std::vector<IPanel *> children;
        const std::string id = "##" + Util::uuidV4();

        void renderChildren() const;

    public:
        void appendChild(AbstractPanel *panel);

        void removeAllChildren() override;

        void setContext(ApplicationContext *context);
    };
}
#endif
