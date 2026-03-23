#include "Util.h"
#include <algorithm>

namespace Metal::Util {
    std::string getTypeName(std::string name, bool toLower) {
        size_t lastSpace = name.find_last_of(' ');
        if (lastSpace != std::string::npos) {
            name = name.substr(lastSpace + 1);
        }
        size_t lastColon = name.find_last_of(':');
        if (lastColon != std::string::npos) {
            name = name.substr(lastColon + 1);
        }
        if (toLower) {
            std::ranges::transform(name, name.begin(),
                                   [](unsigned char c) { return std::tolower(c); });
        }
        return name;
    }

    std::string uuidV4() {
        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        };
        return ss.str();
    }
}
