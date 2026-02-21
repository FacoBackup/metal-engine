#ifndef ABSTRACTIMPORTER_H
#define ABSTRACTIMPORTER_H
#include <stop_token>
#include <string>
#include <vector>

#include "../../common/AbstractRuntimeComponent.h"

namespace Metal {
    class AbstractImporter : public AbstractRuntimeComponent {
    public:
        virtual std::vector<std::string> getSupportedTypes() {
            return {};
        }

        explicit AbstractImporter()
            : AbstractRuntimeComponent() {
        }

        virtual std::string importData(const std::string &targetDir, const std::string &pathToFile,
                                       const std::stop_token &stopToken) {
            return "";
        }

        bool isCompatible(const std::string &path) {
            for (auto &ext: getSupportedTypes()) {
                if (path.find(ext) != std::string::npos) {
                    return true;
                }
            }
            return false;
        }
    };
}
#endif //ABSTRACTIMPORTER_H
