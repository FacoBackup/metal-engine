#ifndef METAL_ENGINE_ABSTRACTREPOSITORY_H
#define METAL_ENGINE_ABSTRACTREPOSITORY_H

#include "../inspection/Inspectable.h"
#include "AbstractSystem.h"

namespace Metal {

    class AbstractRepository : public AbstractSystem, public Inspectable {
    public:
        explicit AbstractRepository(ApplicationContext &context);
    };

}

#endif
