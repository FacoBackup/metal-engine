#include "DockRepository.h"

namespace Metal {
    DockRepository::DockRepository() {
        auto rightUp = DockDTO{{DockSpace::HIERARCHY.index}};
        auto rightDown = DockDTO{{DockSpace::INSPECTOR.index}};
        auto downLeft = DockDTO{{DockSpace::CONSOLE.index}};
        auto downRight = DockDTO{{DockSpace::FILES.index}};

        center.sizeRatioForNodeAtDir = 0.17f;
        rightUp.sizeRatioForNodeAtDir = 0.4f;
        rightDown.sizeRatioForNodeAtDir = 0.6f;
        downLeft.sizeRatioForNodeAtDir = 0.22f;
        downRight.sizeRatioForNodeAtDir = 0.5f;

        right.push_back(rightUp);
        right.push_back(rightDown);

        bottom.push_back(downLeft);
        bottom.push_back(downRight);
    }
} // Metal
