#ifndef IUIRENDERER_H
#define IUIRENDERER_H
#pragma once
namespace backends {
    class IUIRenderer {
    public:
        virtual ~IUIRenderer() = default;

        virtual void start() = 0;

        // Cleanup resources
        virtual void shutdown() = 0;
    private:
        virtual void newFrame() = 0;

        virtual void render() = 0;
    };
}
#endif
