#ifndef FILEMETADATA_H
#define FILEMETADATA_H

#include <string>
#include <nlohmann/json.hpp>
#include "../Util.h"
#include "EntryType.h"
#include "../../interface/Serializable.h"

namespace Metal {
    struct FileMetadata : Serializable {
    protected:
        std::string id = Util::uuidV4();

    public:
        std::string name{};
        EntryType type = EntryType::NONE;

        std::string getId() {
            return id;
        }

        std::string serialize() override {
            const nlohmann::json jsonData = *this;
            return jsonData.dump(0);
        }

        void deserialize(const std::string &j) override {
            const nlohmann::json jsonData = nlohmann::json::parse(j);
            const FileMetadata parsedData = jsonData.get<FileMetadata>();
            this->name = parsedData.name;
            this->id = parsedData.id;
            this->type = parsedData.type;
        }

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(FileMetadata, name, type, id)
    };

}
#endif //FILEMETADATA_H
