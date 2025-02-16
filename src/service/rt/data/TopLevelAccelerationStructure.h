#ifndef TOPLEVELACCELERATIONSTRUCTURE_H
#define TOPLEVELACCELERATIONSTRUCTURE_H

namespace Metal {
    struct TopLevelAccelerationStructure final {
        alignas(4) unsigned int transform;
        alignas(4) unsigned int bottomLevelASOffset;
    };
}
#endif //TOPLEVELACCELERATIONSTRUCTURE_H
