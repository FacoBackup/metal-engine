#include "IntField.h"

namespace Metal {
    void IntField::onSync() {
        Syncronizable::onSync();
    }

    IntField::IntField(InspectedField<int> &field) : field(field) {

    }
} // Metal