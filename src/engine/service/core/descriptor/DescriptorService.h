#ifndef DESCRIPTORPOOLSERVICE_H
#define DESCRIPTORPOOLSERVICE_H
#include "../../AbstractResourceService.h"

namespace Metal {
    struct CommandDescriptorRepository;
    struct DescriptorInstance;

    class DescriptorService final : public AbstractResourceService {
        CommandDescriptorRepository &poolRepository;

    public:
        explicit DescriptorService(ApplicationContext &context);

        DescriptorInstance *createDescritor();

        void onInitialize() override;
    };
} // Metal

#endif //DESCRIPTORPOOLSERVICE_H
