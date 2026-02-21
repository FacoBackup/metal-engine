#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <nlohmann/json.hpp>

namespace Metal {
    class Serializable {
    public:
        virtual ~Serializable() = default;

        virtual nlohmann::json toJson() const = 0;

        virtual void fromJson(const nlohmann::json &j) = 0;

        void saveToJson(const std::string &path) const;

        void loadFromJson(const std::string &path);
    };
}

#endif //SERIALIZABLE_H
