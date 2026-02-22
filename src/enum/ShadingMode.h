#ifndef SHADINGMODE_H
#define SHADINGMODE_H
#include <unordered_map>
#include <string>
#include <vector>

#include "../common/interface/Icons.h"

namespace Metal {
    enum ShadingMode {
        LIT,
        ALBEDO,
        NORMAL,
        ROUGHNESS,
        METALLIC,
        AO,
        RANDOM,
        DEPTH,
        UV,
        POSITION,
        LIGHTING_ONLY,
        EMISSIVE,
        SURFACE_CACHE,
        SURFACE_CACHE_SAMPLES
    };

    class ShadingModes {
    public:
        struct Entry {
            std::string name;
            std::string label;
            std::string icon;
            ShadingMode value;
        };

        inline static const std::vector<Entry> entries = {
                {"LIT",                   "Lit",                   Icons::lightbulb,        LIT},
                {"ALBEDO",                "Albedo",                Icons::image,            ALBEDO},
                {"NORMAL",                "Normal",                Icons::gradient,         NORMAL},
                {"ROUGHNESS",             "Roughness",             Icons::texture,          ROUGHNESS},
                {"METALLIC",              "Metallic",              Icons::blur_on,          METALLIC},
                {"AO",                    "Occlusion",             Icons::contrast,         AO},
                {"RANDOM",                "Random",                Icons::casino,           RANDOM},
                {"DEPTH",                 "Depth",                 Icons::layers,           DEPTH},
                {"UV",                    "UV",                    Icons::grid_on,          UV},
                {"POSITION",              "Position",              Icons::place,            POSITION},
                {"LIGHTING_ONLY",         "Lighting only",         Icons::highlight,        LIGHTING_ONLY},
                {"EMISSIVE",              "Emission",              Icons::wb_incandescent,  EMISSIVE},
                {"SURFACE_CACHE",         "Surface cache",         Icons::ipublic,          SURFACE_CACHE},
                {"SURFACE_CACHE_SAMPLES", "Surface cache samples", Icons::ipublic,          SURFACE_CACHE_SAMPLES}
        };

        static inline std::string GetNames() {
            std::string names;
            for (const auto &entry: entries) {
                names += entry.label + '\0';
            }
            return names;
        }

        static inline std::unordered_map<std::string, unsigned int> getShaderEntries() {
            std::unordered_map<std::string, unsigned int> shaderEntries;
            for (const auto &entry: entries) {
                shaderEntries[entry.name] = entry.value;
            }
            return shaderEntries;
        }

        static inline ShadingMode ValueOfIndex(const int option) {
            if (option >= 0 && option < entries.size()) {
                return entries[option].value;
            }
            return RANDOM;
        }

        static inline int IndexOfValue(const ShadingMode mode) {
            for (int i = 0; i < entries.size(); ++i) {
                if (entries[i].value == mode) {
                    return i;
                }
            }
            return 6; // RANDOM
        }
    };
}

#endif //SHADINGMODE_H
