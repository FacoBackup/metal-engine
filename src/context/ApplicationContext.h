#ifndef METAL_ENGINE_APPLICATIONCONTEXT_H
#define METAL_ENGINE_APPLICATIONCONTEXT_H
#define CACHED_PATH "/metal-engine-cached.txt"
#include <string>

#include "ApplicationSingletons.h"


namespace Metal {
    class ApplicationContext {
        bool debugMode;
        std::string rootDirectory;
        std::unique_ptr<ApplicationSingletons> applicationSingletons = nullptr;
    public:

        ApplicationSingletons &getSingletons() const;

        [[nodiscard]] bool isDebugMode() const { return debugMode; }

        void updateRootPath(bool forceSelection);

        [[nodiscard]] const std::string &getRootDirectory() const {
            return rootDirectory;
        }

        [[nodiscard]] std::string getAssetRefDirectory() const {
            return rootDirectory + "/assets-ref/";
        }

        [[nodiscard]] std::string getShadersDirectory() const {
            return rootDirectory + "/shaders/";
        }

        [[nodiscard]] std::string getAssetDirectory() const {
            return rootDirectory + "/assets/";
        }

        [[nodiscard]] uint32_t getFrameIndex() const;

        void start();

        void save();

        explicit ApplicationContext(const bool debugMode): debugMode(debugMode) {
        }
    };
}

#endif
