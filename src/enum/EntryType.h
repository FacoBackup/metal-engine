#ifndef FILETYPE_H
#define FILETYPE_H

namespace Metal::EntryType {
    enum EntryType {
        SCENE,
        MESH,
        TEXTURE,
        DIRECTORY,
        MATERIAL,
        NONE
    };

    static const char *Names = "Scene\0Mesh\0Texture\0Directory\0Material\0None\0";

    static EntryType ValueOfIndex(const int option) {
        if (option == 0) {
            return EntryType::SCENE;
        }
        if (option == 1) {
            return EntryType::MESH;
        }
        if (option == 2) {
            return EntryType::TEXTURE;
        }
        if (option == 3) {
            return EntryType::DIRECTORY;
        }
        if (option == 4) {
            return EntryType::MATERIAL;
        }
        return EntryType::NONE;
    }

    static int IndexOfValue(const EntryType mode) {
        if (mode == SCENE) {
            return 0;
        }
        if (mode == MESH) {
            return 1;
        }
        if (mode == TEXTURE) {
            return 2;
        }
        if (mode == DIRECTORY) {
            return 3;
        }
        if (mode == MATERIAL) {
            return 4;
        }
        if (mode == NONE) {
            return 5;
        }
        return 5;
    }
}
#endif
