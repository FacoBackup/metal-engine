#include "FloatField.h"

namespace Metal {

    void FloatField::onSync() {
        Syncronizable::onSync();
    }

    FloatField::FloatField(InspectedField<float> &field) : field(field) {
    }
} // Metal