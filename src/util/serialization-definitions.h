#ifndef SERIALIZATION_DEFINITIONS_H
#define SERIALIZATION_DEFINITIONS_H
#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/atomic.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/bitset.hpp>
#include <cereal/types/chrono.hpp>
#include <cereal/types/common.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/forward_list.hpp>
#include <cereal/types/functional.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/queue.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/stack.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/valarray.hpp>
#include <cereal/types/variant.hpp>
#include <filesystem>

#define SAVE_TEMPLATE(...) \
template<class Archive> \
void save(Archive &ar) const { \
ar(__VA_ARGS__); \
}\
template<class Archive>\
void load(Archive &ar) {\
try{\
ar(__VA_ARGS__); \
}catch (std::exception ex) {\
std::cout << "Could not parse data" << std::endl;\
}\
}

#define SERIALIZE_TEMPLATE(...) \
template<class Archive>\
void serialize(Archive &ar) {\
ar(__VA_ARGS__);\
}

#define DUMP_TEMPLATE(PATH, DATA){\
std::ofstream os(PATH, std::ios::binary);\
cereal::BinaryOutputArchive archive(os);\
archive(DATA);\
}

#define PARSE_TEMPLATE(D, P) { \
    if (std::filesystem::exists(P)) {\
        std::cout << "Loading " << P << std::endl;\
        std::ifstream os(P, std::ios::binary);\
        cereal::BinaryInputArchive archive(os);\
        D(archive);\
    }\
}

#define HASH_OF_CLASS_NAME(D) std::to_string(std::hash<std::string>{}(typeid(D).name()))

#endif //SERIALIZATION_DEFINITIONS_H
