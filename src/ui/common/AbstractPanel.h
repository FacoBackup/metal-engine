#ifndef METAL_ENGINE_ABSTRACTPANEL_H
#define METAL_ENGINE_ABSTRACTPANEL_H

#include "IPanel.h"
#include "../../context/ApplicationContext.h"

namespace Metal{
    class AbstractPanel : public IPanel{
    protected:
        ApplicationContext *context = nullptr;
        std::vector<IPanel *> panels;

        void appendChild(AbstractPanel *panel);
    public:
        void setContext(ApplicationContext *context);
    };
}
#endif
