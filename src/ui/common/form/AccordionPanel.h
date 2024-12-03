#ifndef ACCORDION_H
#define ACCORDION_H
#include "../AbstractPanel.h"

namespace Metal {
    class AccordionPanel final : public AbstractPanel {
        const char *fixedId = id.c_str();

    public:
        std::string title;

        void setTitle(const std::string &title);

        void onSync() override;
    };
}

#endif
