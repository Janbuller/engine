#pragma once

#include "engine/utils/DeltaVariable.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>
#include <string>
#include <tuple>

namespace engine::glcore {
    class Window {
    private:
        bool MouseCaptured = true;
        float lastFrame;

        void onWindowResize(int width, int height);
        void onKeyPressed(int key, int scancode, int action, int mods);
        void onMouseButtonPressed(int button, int action, int mods);

        std::function<void(int key, int scandcode, int action, int mods)> KeyPressedCallback;
        std::function<void(int button, int action, int mods)> MouseButtonPressedCallback;

        engine::DeltaVariable<double, 1> deltaTime;

    public:
        int Width = 640, Height = 480;
        std::string Title;

        GLFWwindow *WindowHandle;

        Window(int Width = 640, int Height = 480, std::string Title = "Window");
        ~Window();
        void SwapBuffers();
        void Update();

        double GetDeltaTime();

        bool ShouldClose();
        void SetShouldClose(bool close);

        void CaptureMouse(bool capture);
        bool IsMouseCaptured();

        void SetTitle(std::string title);

        enum class KeyState {
            KEY_PRESS,
            KEY_RELEASE,

            NONE
        };
        KeyState GetKeyState(int key);
        std::array<double, 2> GetCursorPos();

        void SetKeyPressedCallback(std::function<void(int key, int scandcode, int action, int mods)> Callback);
        void SetMouseButtonPressedCallback(std::function<void(int button, int action, int mods)> Callback);
    };
}// namespace engine::glcore
