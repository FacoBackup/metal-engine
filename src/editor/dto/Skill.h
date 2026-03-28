#ifndef SKILL_H
#define SKILL_H

#include <string>
#include <vector>
#include <map>
#include "common/Icons.h"

namespace Metal {
    struct Skill {
        std::string label;
        std::string icon;
        std::string path;
    };

    inline const std::map<std::string, Skill> SKILLS = {
        {"Lua Skill", {"Lua Skill", Icons::star_shine, "resources/prompts/skills/lua-skill.md"}}
    };
}

#endif // SKILL_H
