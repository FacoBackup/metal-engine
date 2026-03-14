#ifndef METAL_ENGINE_APPLICATIONCONTEXT_H
#define METAL_ENGINE_APPLICATIONCONTEXT_H
#include <string>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>
#include <filesystem>
#include <vector>

#include "common/IInit.h"
#include "common/IContextMember.h"
#include "common/IDisposable.h"

#define CTX ApplicationContext::CONTEXT
#define ENGINE_NAME "Metal Engine"

namespace Metal {
    namespace fs = std::filesystem;

    class ApplicationContext : public IInit, public IDisposable {
    public:
        static ApplicationContext *CONTEXT;

    private:
        std::unordered_map<std::string, IContextMember *> singletons;
        std::vector<std::unique_ptr<IContextMember> > instances;
        bool debugMode;

    public:
        explicit ApplicationContext(bool debugMode);

        void onInitialize() override;

        template<typename T>
        void registerSingleton(std::unique_ptr<T> instance) {
            static_assert(std::is_base_of_v<IContextMember, T>, "T must derive from IContextMember");
            T *ptr = instance.get();
            singletons[typeid(T).name()] = static_cast<IContextMember *>(ptr);
            instances.push_back(std::move(instance));
        }

        template<typename T>
        T &getSingleton() {
            auto it = singletons.find(typeid(T).name());
            if (it == singletons.end()) {
                throw std::runtime_error(std::string("Singleton not registered: ") + typeid(T).name());
            }
            return *static_cast<T *>(it->second);
        }

        void *getSingletonByName(const std::string &name) {
            auto it = singletons.find(name);
            if (it == singletons.end()) {
                throw std::runtime_error(std::string("Singleton not registered: ") + name);
            }
            return it->second;
        }

        [[nodiscard]] bool isDebugMode() const;

        void dispose() override;
    };
}

#endif
