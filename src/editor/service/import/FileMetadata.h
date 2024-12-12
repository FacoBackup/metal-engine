#ifndef FILEMETADATA_H
#define FILEMETADATA_H
#include "../../../common/interface/Serializable.h"
#include "../../../common/util/Util.h"

namespace Metal {
    enum class EntryType;

    struct FileMetadata final : Serializable {
        std::string name;
        std::vector<std::string> associatedFiles{};
        EntryType fileType;

        void writeFields(const std::ofstream &ofs) override {
        }

        void readFields(const std::ifstream &ifs) override {
        }

        std::string getId() {
            return id;
        }

    private:
        std::string id = Util::uuidV4();
    };
}
#endif //FILEMETADATA_H
