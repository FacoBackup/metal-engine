#include "Util.h"

#include <typeinfo>
#include <algorithm>

namespace Metal {
    std::string getTypeName(std::string name) {
        size_t lastSpace = name.find_last_of(' ');
        if (lastSpace != std::string::npos) {
            name = name.substr(lastSpace + 1);
        }
        size_t lastColon = name.find_last_of(':');
        if (lastColon != std::string::npos) {
            name = name.substr(lastColon + 1);
        }
        std::ranges::transform(name, name.begin(),
                               [](unsigned char c) { return std::tolower(c); });
        return name;
    }
}
