#ifndef IUIRENDERER_H
#define IUIRENDERER_H
#pragma once
namespace metal::renderer {
    class IUIRenderer {
    public:

        virtual void start() = 0;

        virtual void shutdown() = 0;
    private:
        virtual void newFrame() = 0;

        virtual void render() = 0;
    };
}
#endif
