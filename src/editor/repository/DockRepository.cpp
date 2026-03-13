#include "../../../repository/dock/DockRepository.h"

namespace Metal {
    DockRepository::DockRepository() : AbstractRuntimeComponent() {
        auto *rightT = new DockDTO{&DockSpace::WORLD};
        auto *leftTop = new DockDTO{&DockSpace::REPOSITORIES};
        auto *leftDown = new DockDTO{&DockSpace::INSPECTOR};
        auto *downLeft = new DockDTO{&DockSpace::CONSOLE};
        auto *downRight = new DockDTO{&DockSpace::FILES};

        center.sizeRatioForNodeAtDir = 0.5f;
        rightT->sizeRatioForNodeAtDir = 0.25f;
        leftTop->sizeRatioForNodeAtDir = 0.2f;
        leftDown->sizeRatioForNodeAtDir = 0.5f;
        downLeft->sizeRatioForNodeAtDir = 0.25f;
        downRight->sizeRatioForNodeAtDir = 0.5f;

        right.push_back(rightT);
        left.push_back(leftTop);
        left.push_back(leftDown);

        bottom.push_back(downLeft);
        bottom.push_back(downRight);
    }
} // Metal
