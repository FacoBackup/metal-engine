#ifndef METAL_ENGINE_INSPECTABLEMEMBER_H
#define METAL_ENGINE_INSPECTABLEMEMBER_H

#include <string>
#include <vector>
#include <functional>
#include <optional>
#include <entt/entity/entity.hpp>

#include <nlohmann/json.hpp>
#include "FieldType.h"

namespace Metal {
    class Reflection;
    struct FileExtensionInfo;
    struct FieldMetadata;

    struct IDataTransformer {
        virtual ~IDataTransformer() = default;

        virtual nlohmann::json toJson() const = 0;

        virtual void fromJson(const nlohmann::json &j) const = 0;
    };

    struct LambdaTransformer : IDataTransformer {
        std::function<nlohmann::json()> toJsonFunc;
        std::function<void(const nlohmann::json &)> fromJsonFunc;

        LambdaTransformer(std::function<nlohmann::json()> toJson,
                          std::function<void(const nlohmann::json &)> fromJson)
            : toJsonFunc(std::move(toJson)), fromJsonFunc(std::move(fromJson)) {
        }

        nlohmann::json toJson() const override {
            return toJsonFunc();
        }

        void fromJson(const nlohmann::json &j) const override {
            fromJsonFunc(j);
        }
    };

    enum UsageFlags : uint8_t {
        UI_VISIBLE = 1 << 0,
        SERIALIZABLE = 1 << 1,
        DEFAULT = UI_VISIBLE | SERIALIZABLE
    };

    struct FieldMetadata {
        std::string name;
        std::string nameWithId;
        std::string id;
        std::string group;
        std::string path;
        FieldType type = BOOLEAN;
        Reflection *instance = nullptr;

        void *pointer = nullptr;
        std::function<void()> callback;

        // Metadata
        std::optional<float> min = std::numeric_limits<float>::min();
        std::optional<float> max = std::numeric_limits<float>::max();
        std::optional<float> increment = 1;
        bool disabled = false;
        std::vector<const FileExtensionInfo *> supportedFileTypes{};

        UsageFlags flags = DEFAULT;
        std::shared_ptr<IDataTransformer> transformer = nullptr;

        FieldMetadata() = default;

        virtual ~FieldMetadata() = default;

        FieldMetadata &setMin(float m) {
            min = m;
            return *this;
        }

        FieldMetadata &setMax(float m) {
            max = m;
            return *this;
        }

        FieldMetadata &setIncrement(float i) {
            increment = i;
            return *this;
        }

        FieldMetadata &setDisabled(bool d) {
            disabled = d;
            return *this;
        }

        FieldMetadata &setFlags(UsageFlags f) {
            flags = f;
            return *this;
        }

        FieldMetadata &setName(std::string n) {
            name = std::move(n);
            return *this;
        }

        FieldMetadata &setGroup(std::string g) {
            group = std::move(g);
            return *this;
        }

        FieldMetadata &setSupportedFileTypes(const std::vector<const FileExtensionInfo *> &types) {
            supportedFileTypes = types;
            return *this;
        }

        FieldMetadata &setTransformer(std::function<nlohmann::json()> toJson,
                                      std::function<void(const nlohmann::json &)>
                                      fromJson) {
            transformer = std::make_shared<LambdaTransformer>(std::move(toJson), std::move(fromJson));
            return *this;
        }
    };
}
#endif
