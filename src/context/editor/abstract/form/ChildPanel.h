#ifndef CHILD_H
#define CHILD_H

#include "../AbstractPanel.h"

namespace Metal {
    class ChildPanel final : public AbstractPanel {
        std::string fixedId = id;
        std::string title;

    public:

        void setTitle(const std::string &title);

        void onSync() override;

        void onSyncChildren() const override;
    };
}

#endif
