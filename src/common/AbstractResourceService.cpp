#include "AbstractResourceService.h"
#include "../MetalContextProvider.h"
#include "../common/RuntimeResource.h"

namespace Metal
{
    void AbstractResourceService::registerResource(RuntimeResource* resource)
    {
        resources[resource->getId()] = resource;
    }

    void AbstractResourceService::disposeAll()
    {
        for (auto it = resources.begin(); it != resources.end();)
        {
            auto* r = it->second;
            r->dispose();
            it = resources.erase(it); // Erase and update the iterator
            delete r; // Delete the resource after erasing
        }
    }
}
