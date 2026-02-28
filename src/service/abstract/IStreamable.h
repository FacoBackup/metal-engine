#ifndef ISTREAMABLE_H
#define ISTREAMABLE_H

#include <string>
#include "AbstractResourceService.h"

namespace Metal {
    class RuntimeResource;

    template<typename T>
    class IStreamable : public AbstractResourceService<T> {
    public:
        virtual ~IStreamable() = default;

        virtual T *create(const std::string &id) = 0;
    };
}

#endif //ISTREAMABLE_H
