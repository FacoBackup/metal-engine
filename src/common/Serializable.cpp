#include "../util/Serializable.h"
#include "../ApplicationContext.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include "../context/editor/service/LogService.h"

namespace Metal {
    void Serializable::saveToJson(const std::string &path) const {
        std::ofstream os(path);
        if (os.is_open()) {
            os << toJson().dump(4);
        }
    }

    void Serializable::loadFromJson(const std::string &path) {
        if (std::filesystem::exists(path)) {
            std::ifstream is(path);
            if (is.is_open()) {
                try {
                    nlohmann::json j;
                    is >> j;
                    fromJson(j);
                } catch (const std::exception &e) {
                    LOG_ERROR(std::format("Error parsing JSON from {}{} {}", path, ": ", e.what()));
                }
            }
        }
    }
}
