#ifndef SERIALIZATION_DEFINITIONS_H
#define SERIALIZATION_DEFINITIONS_H
#include <filesystem>
#include <fstream>
#include <iostream>
#include "Serializable.h"

#define DUMP_TEMPLATE(PATH, DATA){\
    (DATA).saveToJson(PATH);\
}

#define PARSE_TEMPLATE(D, P) { \
    if (std::filesystem::exists(P)) {\
        std::cout << "Loading " << P << std::endl;\
        (D).loadFromJson(P);\
    }\
}

#define HASH_OF_CLASS_NAME(D) std::to_string(std::hash<std::string>{}(typeid(D).name()))

#endif //SERIALIZATION_DEFINITIONS_H
