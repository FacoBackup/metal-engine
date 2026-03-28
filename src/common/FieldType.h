#ifndef FIELDTYPE_H
#define FIELDTYPE_H

#include <glm/fwd.hpp>
#include <string>

namespace Metal {
    class Reflection;

    enum FieldType {
        COMPOSITE,
        RESOURCE,
        COLOR,
        STRING,
        INT,
        UINT,
        FLOAT,
        BOOLEAN,
        VECTOR2,
        VECTOR3,
        VECTOR4,
        MAT4,
        QUAT,
        GENERIC
    };

    template<FieldType T>
    struct TypeToCpp;

    template<>
    struct TypeToCpp<BOOLEAN> {
        using Type = bool;
    };
    template<>
    struct TypeToCpp<INT> {
        using Type = int;
    };
    template<>
    struct TypeToCpp<UINT> {
        using Type = unsigned int;
    };
    template<>
    struct TypeToCpp<FLOAT> {
        using Type = float;
    };
    template<>
    struct TypeToCpp<STRING> {
        using Type = std::string;
    };
    template<>
    struct TypeToCpp<RESOURCE> {
        using Type = std::string;
    };
    template<>
    struct TypeToCpp<VECTOR2> {
        using Type = glm::vec2;
    };
    template<>
    struct TypeToCpp<VECTOR3> {
        using Type = glm::vec3;
    };
    template<>
    struct TypeToCpp<COLOR> {
        using Type = glm::vec3;
    };
    template<>
    struct TypeToCpp<VECTOR4> {
        using Type = glm::vec4;
    };
    template<>
    struct TypeToCpp<MAT4> {
        using Type = glm::mat4;
    };
    template<>
    struct TypeToCpp<QUAT> {
        using Type = glm::quat;
    };
    template<>
    struct TypeToCpp<COMPOSITE> {
        using Type = Reflection;
    };
    template<>
    struct TypeToCpp<GENERIC> {
        using Type = void;
    };
}
#endif
