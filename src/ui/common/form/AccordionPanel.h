#ifndef ACCORDION_H
#define ACCORDION_H

#include "../AbstractPanel.h"

namespace Metal {
    class AccordionPanel final : public AbstractPanel {
        std::string fixedId = id;
        std::string title;

    public:

        void setTitle(const std::string &title);

        void onSync() override;
    };
}

#endif
