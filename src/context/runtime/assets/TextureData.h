#ifndef TEXTUREDATA_H
#define TEXTUREDATA_H

namespace Metal {
    struct TextureData {
        int width;
        int height;
        int channels;
        unsigned char *data;
    };
}
#endif //TEXTUREDATA_H
