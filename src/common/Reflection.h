#ifndef INSPECTABLE_H
#define INSPECTABLE_H

#include <functional>
#include <string>
#include <vector>
#include <glm/fwd.hpp>

#include <nlohmann/json.hpp>
#include "FieldMetadata.h"
#include "Util.h"

namespace Metal {
    struct FileExtensionInfo;

    class Reflection {
        std::string uniqueIdentifier = Util::uuidV4();
        std::vector<std::shared_ptr<FieldMetadata> > fields{};
        bool fieldsRegistered = false;

    public:
        Reflection() = default;

        Reflection(const Reflection &other)
            : uniqueIdentifier(other.uniqueIdentifier) {
        }

        virtual ~Reflection() = default;

    protected:
        template<FieldType FT>
        FieldMetadata &registerSerializableOnlyField(void *pointer) {
            static_assert(FT != COMPOSITE && FT != GENERIC,
                          "registerSerializableOnlyField does not support COMPOSITE or GENERIC. "
                          "Use registerCompositeField or registerGenericField instead.");
            return registerFieldInternal(pointer, FT, SERIALIZABLE);
        }

        template<FieldType FT, typename T>
        FieldMetadata &registerSerializableOnlyField(T *pointer) {
            static_assert(FT != COMPOSITE && FT != GENERIC,
                          "registerSerializableOnlyField does not support COMPOSITE or GENERIC. "
                          "Use registerCompositeField or registerGenericField instead.");
            if constexpr (FT != COMPOSITE && FT != GENERIC) {
                static_assert(std::is_same_v<typename TypeToCpp<FT>::Type, T>, "Field type mismatch");
            }
            return registerFieldInternal(static_cast<void *>(pointer), FT, SERIALIZABLE);
        }

        template<FieldType FT>
        FieldMetadata &registerEditableField(void *pointer) {
            static_assert(FT != COMPOSITE && FT != GENERIC,
                          "registerEditableField does not support COMPOSITE or GENERIC. "
                          "Use registerCompositeField or registerGenericField instead.");
            return registerFieldInternal(pointer, FT, DEFAULT);
        }

        template<FieldType FT, typename T>
        FieldMetadata &registerEditableField(T *pointer) {
            static_assert(FT != COMPOSITE && FT != GENERIC,
                          "registerEditableField does not support COMPOSITE or GENERIC. "
                          "Use registerCompositeField or registerGenericField instead.");
            if constexpr (FT != COMPOSITE && FT != GENERIC) {
                static_assert(std::is_same_v<typename TypeToCpp<FT>::Type, T>, "Field type mismatch");
            }
            return registerFieldInternal(static_cast<void *>(pointer), FT, DEFAULT);
        }

        template<typename T>
        FieldMetadata &registerCompositeField(T *pointer, UsageFlags flags = SERIALIZABLE) {
            static_assert(std::is_base_of_v<Reflection, T>, "Field type must extend Reflection");
            return registerFieldInternal(static_cast<void *>(pointer), COMPOSITE, flags);
        }

        FieldMetadata &registerGenericField(std::function<nlohmann::json()> toJson,
                                           std::function<void(const nlohmann::json &)> fromJson,
                                           UsageFlags flags = SERIALIZABLE) {
            auto &field = registerFieldInternal(nullptr, GENERIC, flags);
            field.setTransformer(std::move(toJson), std::move(fromJson));
            return field;
        }

    private:
        FieldMetadata &registerFieldInternal(void *pointer, FieldType type, UsageFlags flags);

    public:
        virtual void registerFields() {
        }

        [[nodiscard]] nlohmann::json toJson(UsageFlags filter = SERIALIZABLE, bool isVerbose = false);

        void fromJson(const nlohmann::json &j);

    private:
        void applyValueFromJson(const std::shared_ptr<FieldMetadata> &field, const nlohmann::json &val);

    public:
        // Virtual for testing
        [[nodiscard]] virtual std::string getClassName() const;

        std::shared_ptr<FieldMetadata> getFieldByPointer(void *ptr);

        std::shared_ptr<FieldMetadata> getFieldByPath(const std::string &path);

        std::vector<std::shared_ptr<FieldMetadata> > getFields(std::optional<UsageFlags> filter = std::nullopt);

        nlohmann::json getFieldValue(UsageFlags filter, bool isVerbose,
                                     const std::vector<std::shared_ptr<FieldMetadata> >::value_type &field) const;


        [[nodiscard]] std::string getUniqueId() const {
            return uniqueIdentifier;
        }

        virtual const char *getIcon() const {
            return nullptr;
        }

        virtual const char *getTitle() const {
            return nullptr;
        }
    };
}
#endif
