#ifndef METALCONTEXTPROVIDER_H
#define METALCONTEXTPROVIDER_H

#include "context/ApplicationContext.h"
#define ENGINE_NAME "Metal Engine"
#define SINGLETONS ContextProvider::Get().getSingletons()

namespace Metal
{
    class ContextProvider
    {
        static std::unique_ptr<ApplicationContext> instance;

    public:
        static void Initialize(bool debugMode = false);

        static ApplicationContext& Get();
    };
}
#endif // METALCONTEXTPROVIDER_H
