#ifndef ABSTRACTIMPORTER_H
#define ABSTRACTIMPORTER_H
#include <string>
#include <vector>

#include "../../../common/interface/AbstractRuntimeComponent.h"

namespace Metal {
    class AbstractImporter : public AbstractRuntimeComponent {
    protected:
        virtual std::vector<std::string> getSupportedTypes() {
            return {};
        }

    public:
        explicit AbstractImporter(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        bool isCompatible(const std::string &path) {
            for(auto &ext: getSupportedTypes()) {
                if (path.find(ext) != std::string::npos) {
                    return true;
                }
            }
            return false;
        }
    };
}
#endif //ABSTRACTIMPORTER_H
