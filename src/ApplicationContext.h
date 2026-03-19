#ifndef METAL_ENGINE_APPLICATIONCONTEXT_H
#define METAL_ENGINE_APPLICATIONCONTEXT_H
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <string>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>
#include <filesystem>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype>

#include "common/IInit.h"
#include "common/IContextMember.h"
#include "common/IDisposable.h"
#include "common/Util.h"

#define ENGINE_NAME "Metal Engine"

namespace Metal {
    namespace fs = std::filesystem;

    class ApplicationContext : public IContextMember, public IInit, public IDisposable {
        std::unordered_map<std::string, IContextMember *> singletons;
        std::vector<std::shared_ptr<IContextMember> > instances;
        bool debugMode;


    public:
        explicit ApplicationContext(bool debugMode);

        void onInitialize() override;

        template<typename T>
        void registerSingleton(std::shared_ptr<T> instance) {
            static_assert(std::is_base_of_v<IContextMember, T>, "T must derive from IContextMember");
            T *ptr = instance.get();
            singletons[getTypeName(typeid(T).name())] = static_cast<IContextMember *>(ptr);
            instances.push_back(std::move(instance));
        }

        template<typename T>
        T &getSingleton() {
            auto name = getTypeName(typeid(T).name());
            auto it = singletons.find(name);
            if (it == singletons.end()) {
                throw std::runtime_error(std::string("Singleton not registered: ") + name);
            }
            return *static_cast<T *>(it->second);
        }

        void *getSingletonByName(const std::string &name) {
            std::string lowerName = name;
            std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                           [](unsigned char c) { return std::tolower(c); });
            auto it = singletons.find(lowerName);
            if (it == singletons.end()) {
                throw std::runtime_error(std::string("Singleton not registered: ") + lowerName);
            }
            return it->second;
        }

        void injectDependencies(IContextMember *member) {
            std::cout << "Injecting dependencies for: " << typeid(*member).name() << std::endl;
            member->setDependencies(*this);
        }

        [[nodiscard]] bool isDebugMode() const;

        [[nodiscard]] const std::vector<std::shared_ptr<IContextMember> > &getInstances() const { return instances; }

        void dispose() override;
    };
}

#endif
