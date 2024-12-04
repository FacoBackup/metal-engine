#include "Inspectable.h"

#include <utility>
#include "InspectedMethod.h"
#include "../Util.h"

namespace Metal {

    void Inspectable::registerMethod(const std::function<void()> &updateCallback, std::string name, std::string group) {
        std::unique_ptr<InspectedMethod> field = std::make_unique<InspectedMethod>();
        field->group = std::move(group);
        field->name = std::move(name);
        field->nameWithId = field->name + Metal::Util::uuidV4();


        field->callback = updateCallback;
        field->type = FieldType::METHOD;
        fields.push_back(std::move(field));
    }

    std::vector<std::unique_ptr<InspectableMember>> &Inspectable::getFields() {
        if (!fieldsRegistered) {
            fieldsRegistered = true;
            registerFields();
        }
        return fields;
    }

    void Inspectable::registerInt(int &v,
                                  std::string group, std::string name,
                                  int min, int max, bool disabled) {
        std::unique_ptr<InspectedField<int>> field = std::make_unique<InspectedField<int>>(v);

        field->name = std::move(name);
        field->nameWithId = field->name + Metal::Util::uuidV4();
        field->type = INT;
        field->min = min;
        field->max = max;
        field->group = std::move(group);
        field->disabled = disabled;

        fields.push_back(std::move(field));

    }

    void Inspectable::registerFloat(float &v,
                                    std::string group, std::string name,
                                    float min, float max, bool disabled) {
        std::unique_ptr<InspectedField<float>> field = std::make_unique<InspectedField<float>>(v);


        field->name = std::move(name);
        field->nameWithId = field->name + Metal::Util::uuidV4();

        field->type = FLOAT;
        field->minF = min;
        field->maxF = max;
        field->group = std::move(group);
        field->disabled = disabled;

        fields.push_back(std::move(field));

    }

    void Inspectable::registerBool(bool &v,
                                   std::string group, std::string name, bool disabled) {
        std::unique_ptr<InspectedField<bool>> field = std::make_unique<InspectedField<bool>>(v);

        field->name = std::move(name);
        field->nameWithId = field->name + Metal::Util::uuidV4();

        field->type = BOOLEAN;

        field->group = std::move(group);
        field->disabled = disabled;

        fields.push_back(std::move(field));
    }
}
