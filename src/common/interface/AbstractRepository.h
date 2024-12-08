#ifndef METAL_ENGINE_ABSTRACTREPOSITORY_H
#define METAL_ENGINE_ABSTRACTREPOSITORY_H

#include "../inspection/Inspectable.h"
#include "AbstractRuntimeComponent.h"

namespace Metal {

    class AbstractRepository : public AbstractRuntimeComponent, public Inspectable {
    public:
        explicit AbstractRepository(ApplicationContext &context);
    };

}

#endif
