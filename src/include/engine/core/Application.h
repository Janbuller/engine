#pragma once

#include "engine/glcore/Window.h"
#include <string>

namespace engine {
    class Application {
    protected:
        glcore::Window AppWindow;
        double DeltaTime;

    public:
        Application(int width, int height, std::string title);
        void Run();

    public:
        virtual void onCreate();

        virtual bool onUpdate();

        virtual void onExit();

        virtual void onKeyPressed(int key, int scancode, int action, int mods);
        virtual void onMouseButtonPressed(int button, int action, int mods);
        virtual void onWindowResize(int width, int height);
    };
}// namespace engine
