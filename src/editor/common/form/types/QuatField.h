#ifndef QUATFIELD_H
#define QUATFIELD_H
#include <glm/fwd.hpp>
#include "../../AbstractPanel.h"
#include "../../../../common/inspection/InspectedField.h"

namespace Metal {
    class QuatField final : public AbstractPanel {
        InspectedField<glm::quat> &field;
        float values[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    public:
        explicit QuatField(InspectedField<glm::quat> &field);

        void onSync() override;
    };
} // Metal

#endif //QUATFIELD_H
