#ifndef METAL_ENGINE_ILOADER_H
#define METAL_ENGINE_ILOADER_H
#include <string>

namespace Metal {
    class ILoader {
    public:
        virtual ~ILoader() = default;

        virtual void load(const std::string &absolutePath) = 0;

        virtual bool isCompatible(const std::string &absolutePath) = 0;
    };
}
#endif //METAL_ENGINE_ILOADER_H
