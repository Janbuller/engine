#include "engine/core/Application.h"
#include "engine/core/Logger.h"
#include "engine/glcore/Window.h"
#include <iostream>
#include <string>

namespace engine {
    Application::Application(int width, int height, std::string title) : AppWindow{width, height, title} {
        ::engine::Logger::Init();
        LOG_ENGINE_TRACE("Logger initialized!");

        auto keyCallback = [this](int key, int scancode, int action, int mods) {
            onKeyPressed(key, scancode, action, mods);
        };
        AppWindow.SetKeyPressedCallback(keyCallback);


        auto mouseButtonCallback = [this](int button, int action, int mods) {
            onMouseButtonPressed(button, action, mods);
        };
        AppWindow.SetMouseButtonPressedCallback(mouseButtonCallback);
        LOG_ENGINE_TRACE("Key- and MouseButton-Callbacks created.");
    }

    void Application::Run() {
        LOG_ENGINE_TRACE("Application has been run.");
        onCreate();
        while (!AppWindow.ShouldClose()) {
            AppWindow.Update();

            DeltaTime = AppWindow.GetDeltaTime();
            if (!onUpdate()) {
                AppWindow.SetShouldClose(true);
            }

            AppWindow.SwapBuffers();
        }

        onExit();
        LOG_ENGINE_TRACE("Application has been exited.");
    }

    void Application::onCreate() {
    }
    bool Application::onUpdate() {
        glClearColor(0.2, 0.3, 0.1, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Key 256 is the escape key.
        // This is used to avoid pulling in the glfw headers
        if (AppWindow.GetKeyState(256) == glcore::Window::KeyState::KEY_PRESS) {
            return false;
        }

        return true;
    }
    void Application::onExit() {
    }

    void Application::onKeyPressed(int key, int scancode, int action, int mods) {
    }

    void Application::onMouseButtonPressed(int button, int action, int mods) {
    }
}// namespace engine
