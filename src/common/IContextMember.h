#ifndef METAL_ENGINE_CONTEXTMEMBER_H
#define METAL_ENGINE_CONTEXTMEMBER_H
#include <vector>
#include <string>

namespace Metal {
    class ApplicationContext;

    struct Dependency {
        std::string name;
        void *ptr;
    };

    class IContextMember {
    public:
        virtual ~IContextMember() = default;

        /**
         * Dependency injection. Called after all the context is created and
         * will be the data used to initialize the pointers
         */
        virtual std::vector<Dependency> getDependencies() {
            return {};
        }

        /**
         * Will call getDependencies and set the pointers based on the singletons returned
         */
        void setDependencies(ApplicationContext &ctx);
    };
}
#endif //METAL_ENGINE_CONTEXTMEMBER_H
