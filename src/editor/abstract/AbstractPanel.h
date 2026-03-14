#ifndef METAL_ENGINE_ABSTRACTPANEL_H
#define METAL_ENGINE_ABSTRACTPANEL_H

#include <string>
#include <vector>

#include "../../common/IContextMember.h"
#include "../../common/ISync.h"
#include "../../common/IInit.h"

namespace Metal {
    class ApplicationContext;
    class AbstractPanel : public IContextMember, public ISync, public IInit  {
    protected:
        std::vector<AbstractPanel *> children;
        const std::string id;

        virtual void onSyncChildren() const;

    public:
        explicit AbstractPanel();

        std::vector<AbstractPanel *> &getChildren();

        void appendChild(AbstractPanel *panel);

        void removeAllChildren();

        virtual void onRemove() {
        }
    };
}
#endif
