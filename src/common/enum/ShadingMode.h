#ifndef SHADINGMODE_H
#define SHADINGMODE_H

namespace Metal::ShadingMode {
    enum ShadingMode {
        LIT,
        ALBEDO,
        NORMAL,
        ROUGHNESS,
        METALLIC,
        RANDOM,
        UV,
        POSITION
    };

    static auto Names =
            "Lit\0Albedo\0Normal\0Roughness\0Metallic\0Random\0UV\0Position\0";

    static ShadingMode ValueOfIndex(const int option) {
        if (option == 0) {
            return LIT;
        }
        if (option == 1) {
            return ALBEDO;
        }
        if (option == 2) {
            return NORMAL;
        }
        if (option == 3) {
            return ROUGHNESS;
        }
        if (option == 4) {
            return METALLIC;
        }
        if (option == 5) {
            return RANDOM;
        }
        if (option == 6) {
            return UV;
        }
        if (option == 7) {
            return POSITION;
        }
        return POSITION;
    }

    static int IndexOfValue(const ShadingMode mode) {
        if (mode == LIT) {
            return 0;
        }
        if (mode == ALBEDO) {
            return 1;
        }
        if (mode == NORMAL) {
            return 2;
        }
        if (mode == ROUGHNESS) {
            return 3;
        }
        if (mode == METALLIC) {
            return 4;
        }
        if (mode == RANDOM) {
            return 5;
        }
        if (mode == UV) {
            return 6;
        }
        if (mode == POSITION) {
            return 7;
        }
        return 7;
    }
}

#endif //SHADINGMODE_H
