#ifndef CHANGEABLE_H
#define CHANGEABLE_H

namespace Metal {
    class Changeable {
    protected:
        unsigned long changes = 0;
        unsigned long frozenVersion = 99999;

    public:
        [[nodiscard]] unsigned long getChangeId() const {
            return changes;
        }

        void registerChange() {
            changes++;
        }

        [[nodiscard]] bool isNotFrozen() const {
            return frozenVersion != getChangeId();
        }

        void freezeVersion() {
            frozenVersion = getChangeId();
        }
    };
}
#endif
