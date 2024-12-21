#ifndef SHADINGMODE_H
#define SHADINGMODE_H

namespace Metal::ShadingMode {
    enum ShadingMode {
        LIT,
        ALBEDO,
        NORMAL,
        ROUGHNESS,
        METALLIC,
        AO,
        RANDOM
    };

    static auto Names = "Lit\0Albedo\0Normal\0Roughness\0Metallic\0AO\0Random\0";

    static ShadingMode ValueOfIndex(const int option) {
        if (option == 0) {
            return ShadingMode::LIT;
        }
        if (option == 1) {
            return ShadingMode::ALBEDO;
        }
        if (option == 2) {
            return ShadingMode::NORMAL;
        }
        if (option == 3) {
            return ShadingMode::ROUGHNESS;
        }
        if (option == 4) {
            return ShadingMode::METALLIC;
        }
        if (option == 5) {
            return ShadingMode::AO;
        }
        return ShadingMode::RANDOM;
    }

    static int IndexOfValue(const ShadingMode mode) {
        if (mode == ShadingMode::LIT) {
            return 0;
        }
        if (mode == ShadingMode::ALBEDO) {
            return 1;
        }
        if (mode == ShadingMode::NORMAL) {
            return 2;
        }
        if (mode == ShadingMode::ROUGHNESS) {
            return 3;
        }
        if (mode == ShadingMode::METALLIC) {
            return 4;
        }
        if (mode == ShadingMode::AO) {
            return 5;
        }
        return 6;
    }
}

#endif //SHADINGMODE_H
