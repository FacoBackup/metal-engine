#ifndef CHILD_H
#define CHILD_H

#include "AbstractFormFieldPanel.h"

namespace Metal {
    class ChildPanel final : public AbstractFormFieldPanel {
        std::string fixedId = id;
        std::string title;

    public:

        void setTitle(const std::string &title);

        void onSync() override;

        bool isVisible() const override;
    };
}

#endif
