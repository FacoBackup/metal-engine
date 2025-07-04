#ifndef ABSTRACTIMPORTER_H
#define ABSTRACTIMPORTER_H
#include <string>
#include <vector>

#include "AbstractRuntimeComponent.h"

namespace Metal {
    class AbstractImporter : public AbstractRuntimeComponent {
    protected:
        virtual std::vector<std::string> getSupportedTypes() {
            return {};
        }

    public:

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
