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
        Reflection(const Reflection &other)
            : uniqueIdentifier(other.uniqueIdentifier),
              fields(other.fields),
              fieldsRegistered(other.fieldsRegistered) {
        }

    protected:
        FieldMetadata &registerSerializableOnlyField(void *pointer, FieldType type, std::string name);

        FieldMetadata &registerEditableField(void *pointer, FieldType type, std::string name, std::string group);

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

        Reflection() = default;

        virtual ~Reflection() = default;

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
