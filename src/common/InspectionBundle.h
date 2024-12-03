#ifndef INSPECTIONBUNDLE_H
#define INSPECTIONBUNDLE_H
#include <vector>

#include "InspectedField.h"
#include "Inspectable.h"

namespace Metal {
    struct InspectionBundle {
        std::vector<InspectedField> &fields;
        Inspectable *inspected;
        InspectionBundle(std::vector<InspectedField> &fields, Inspectable *inspected);
    };
}
#endif
