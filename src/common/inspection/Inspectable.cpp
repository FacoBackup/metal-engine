#include "Inspectable.h"

#include <utility>
#include "InspectedMethod.h"
#include "../Util.h"

namespace Metal {

    void Inspectable::registerMethod(const std::function<void()> &updateCallback, std::string name, std::string group) {
        InspectedMethod field;
        field.group = std::move(group);
        field.name = std::move(name);
        field.nameWithId = name + Metal::Util::uuidV4();

        field.callback = updateCallback;
        field.type = FieldType::METHOD;
        fields.push_back(field);
    }

    std::vector<InspectableMember> &Inspectable::getFields() {
        if (fields.empty()) {
            registerFields();
        }
        return fields;
    }

    void Inspectable::registerInt(int &v,
                                  std::string group, std::string name,
                                  int min, int max, bool disabled) {
        InspectedField<int> field{v};

        field.name = std::move(name);
        field.nameWithId = name + Metal::Util::uuidV4();
        field.type = FLOAT;
        field.min = min;
        field.max = max;
        field.group = std::move(group);
        field.disabled = disabled;

        fields.push_back(field);
    }

    void Inspectable::registerFloat(float &v,
                                    std::string group, std::string name,
                                    int min, int max, bool disabled) {
        InspectedField<float> field{v};

        field.name = std::move(name);
        field.nameWithId = name + Metal::Util::uuidV4();
        field.type = FLOAT;
        field.min = min;
        field.max = max;
        field.group = std::move(group);
        field.disabled = disabled;

        fields.push_back(field);
    }

    void Inspectable::registerBool(bool &v,
                                   std::string group, std::string name, bool disabled) {
        InspectedField<bool> field{v};

        field.name = std::move(name);
        field.nameWithId = name + Metal::Util::uuidV4();
        field.type = BOOLEAN;

        field.group = std::move(group);
        field.disabled = disabled;

        fields.push_back(field);
    }
}
