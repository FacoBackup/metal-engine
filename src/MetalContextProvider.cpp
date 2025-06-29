#include "MetalContextProvider.h"

namespace Metal
{
    std::unique_ptr<ApplicationContext> ContextProvider::instance;

    void ContextProvider::Initialize(bool debugMode)
    {
        if (instance == nullptr)
        {
            instance = std::make_unique<ApplicationContext>(debugMode);
            instance->start();
        }
    }

    ApplicationContext& ContextProvider::Get()
    {
        if (instance == nullptr)
        {
            throw std::runtime_error("ApplicationContext not initialized. Call Initialize() first.");
        }
        return *instance;
    }
}
