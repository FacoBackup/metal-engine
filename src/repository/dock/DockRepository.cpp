#include "DockRepository.h"

namespace Metal {
    DockRepository::DockRepository() {
        auto rightUp = DockDTO{{DockSpace::HIERARCHY.index}};
        auto rightDown = DockDTO{{DockSpace::INSPECTOR.index}};
        auto downRight = DockDTO{{DockSpace::FILES.index, DockSpace::CANVAS.index}};

        center.sizeRatioForNodeAtDir = .25f;
        rightUp.sizeRatioForNodeAtDir = 0.4f;
        rightDown.sizeRatioForNodeAtDir = 0.6f;
        downRight.sizeRatioForNodeAtDir = .25f;

        right.push_back(rightUp);
        right.push_back(rightDown);

        bottom.push_back(downRight);
    }
} // Metal
