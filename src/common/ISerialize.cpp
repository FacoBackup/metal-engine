#include "ISerialize.h"
#include "../ApplicationContext.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include "LoggerUtil.h"

namespace Metal {
    void ISerialize::saveToJson(const std::string &path) const {
        std::ofstream os(path);
        if (os.is_open()) {
            os << toJson().dump(4);
        }
    }

    void ISerialize::loadFromJson(const std::string &path) {
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
