#ifndef METAL_ENGINE_LOADINGSERVICE_H
#define METAL_ENGINE_LOADINGSERVICE_H

#include <functional>
#include <stop_token>
#include <string>

#include "../../common/AbstractRuntimeComponent.h"

namespace Metal {
    class FileImporterService final : public AbstractRuntimeComponent {
    public:
        explicit FileImporterService(ApplicationContext &context)
            : AbstractRuntimeComponent(context) {
        }

        using LoadingTask = std::function<void(const std::stop_token &)>;

        void importFile(const std::string &targetDir, const std::string &file) const;

        std::string runAsync(const std::string &taskName, const LoadingTask &task) const;

        std::string collectCompatibleFiles() const;
    };
}

#endif
