#ifndef FILEMETADATA_H
#define FILEMETADATA_H

#include <string>
#include "../../util/Util.h"
#include "../../enum/EntryType.h"
#include "../../util/Serializable.h"

namespace Metal {
    struct EntryMetadata : Serializable {
    protected:
        std::string id = Util::uuidV4();

    public:
        std::string name;
        EntryType::EntryType type = EntryType::NONE;

        std::string getId() {
            return id;
        }

        nlohmann::json toJson() const override {
            nlohmann::json j;
            j["name"] = name;
            j["id"] = id;
            j["type"] = (int)type;
            return j;
        }

        void fromJson(const nlohmann::json& j) override {
            name = j.at("name").get<std::string>();
            id = j.at("id").get<std::string>();
            type = (EntryType::EntryType)j.at("type").get<int>();
        }
    };
}
#endif //FILEMETADATA_H
