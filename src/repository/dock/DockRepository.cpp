#include "DockRepository.h"

namespace Metal {
    DockRepository::DockRepository(ApplicationContext &context) : AbstractRuntimeComponent(context) {
        auto *rightUp = new DockDTO{&DockSpace::HIERARCHY};
        auto *rightDown = new DockDTO{&DockSpace::INSPECTOR};
        auto *downLeft = new DockDTO{&DockSpace::CANVAS};
        auto *downRight = new DockDTO{&DockSpace::FILES};

        center.sizeRatioForNodeAtDir = 0.17f;
        rightUp->sizeRatioForNodeAtDir = 0.4f;
        rightDown->sizeRatioForNodeAtDir = 0.6f;
        downLeft->sizeRatioForNodeAtDir = 0.22f;
        downRight->sizeRatioForNodeAtDir = 0.5f;

        right.push_back(rightUp);
        right.push_back(rightDown);

        bottom.push_back(downLeft);
        bottom.push_back(downRight);
    }
} // Metal
