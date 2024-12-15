#ifndef METAL_ENGINE_ABSTRACTPANEL_H
#define METAL_ENGINE_ABSTRACTPANEL_H

#include <string>
#include <vector>

#include "IPanel.h"

namespace Metal {
    class ApplicationContext;

    class AbstractPanel : public IPanel {
    protected:
        ApplicationContext *context = nullptr;
        std::vector<IPanel *> children;
        const std::string id;

        void onSyncChildren() const;

    public:
        explicit AbstractPanel();

        std::vector<IPanel *> &getChildren();

        void appendChild(AbstractPanel *panel);

        void removeAllChildren() override;

        void setContext(ApplicationContext *context);

        virtual void onRemove() {
        }
    };
}
#endif
