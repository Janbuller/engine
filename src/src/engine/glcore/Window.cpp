#include "engine/glcore/Window.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <tuple>

namespace engine::glcore {
    void Window::onWindowResize(int width, int height) {
        this->Width  = width;
        this->Height = height;

        glViewport(0, 0, width, height);
    }

    void Window::onKeyPressed(int key, int scancode, int action, int mods) {
        if (KeyPressedCallback != nullptr) {
            KeyPressedCallback(key, scancode, action, mods);
        }
    }

    void Window::onMouseButtonPressed(int button, int action, int mods) {
        if (MouseButtonPressedCallback != nullptr) {
            MouseButtonPressedCallback(button, action, mods);
        }
    }

    Window::Window(int Width, int Height, std::string Title) : Width(Width), Height(Height), Title(Title),
                                                               deltaTime(std::array<double, 1>{glfwGetTime()}) {
        if (!glfwInit())
            throw -1;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        WindowHandle = glfwCreateWindow(Width, Height, Title.c_str(), NULL, NULL);
        if (!WindowHandle) {
            glfwTerminate();
            throw -1;
        }

        glfwMakeContextCurrent(WindowHandle);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD." << std::endl;
            throw -1;
        }

        glViewport(0, 0, Width, Height);
        lastFrame = glfwGetTime();

        // Use c++ lambda function as callback for glfw, forwarding it to
        // the onWindowResize function of this class, since glfw is a
        // c-library and doesn't support methods.

        glfwSetWindowUserPointer(WindowHandle, this);

        auto windowSizeCallback = [](GLFWwindow *w, int width, int height) {
            static_cast<Window *>(glfwGetWindowUserPointer(w))->onWindowResize(width, height);
        };
        glfwSetWindowSizeCallback(WindowHandle, windowSizeCallback);

        auto keyCallback = [](GLFWwindow *w, int key, int scancode, int action, int mods) {
            static_cast<Window *>(glfwGetWindowUserPointer(w))->onKeyPressed(key, scancode, action, mods);
        };
        glfwSetKeyCallback(WindowHandle, keyCallback);

        auto mouseButtonCallback = [](GLFWwindow *w, int button, int action, int mods) {
            static_cast<Window *>(glfwGetWindowUserPointer(w))->onMouseButtonPressed(button, action, mods);
        };
        glfwSetMouseButtonCallback(WindowHandle, mouseButtonCallback);
    }

    Window::~Window() {
        glfwDestroyWindow(WindowHandle);
        glfwTerminate();
    }

    void Window::SwapBuffers() {
        glfwSwapBuffers(WindowHandle);
        glfwPollEvents();
    }

    void Window::Update() {
    }

    double Window::GetDeltaTime() {
        return deltaTime.GetDelta({glfwGetTime()})[0];
    }

    bool Window::ShouldClose() {
        return glfwWindowShouldClose(WindowHandle);
    }

    void Window::SetShouldClose(bool close) {
        glfwSetWindowShouldClose(WindowHandle, close);
    }


    void Window::CaptureMouse(bool capture) {
        if (capture)
            glfwSetInputMode(WindowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(WindowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    bool Window::IsMouseCaptured() {
        int inputMode = glfwGetInputMode(WindowHandle, GLFW_CURSOR);
        return inputMode == GLFW_CURSOR_NORMAL ? false : true;
    }

    void Window::SetTitle(std::string title) {
        glfwSetWindowTitle(WindowHandle, title.c_str());
    }

    Window::KeyState Window::GetKeyState(int key) const {
        int state = glfwGetKey(WindowHandle, key);

        if (state == GLFW_PRESS)
            return Window::KeyState::KEY_PRESS;
        if (state == GLFW_RELEASE)
            return Window::KeyState::KEY_RELEASE;

        return Window::KeyState::NONE;
    }

    std::array<double, 2> Window::GetCursorPos() const {
        double x, y;
        glfwGetCursorPos(WindowHandle, &x, &y);

        return {x, y};
    }


    void Window::SetKeyPressedCallback(std::function<void(int key, int scandcode, int action, int mods)> Callback) {
        KeyPressedCallback = Callback;
    }

    void Window::SetMouseButtonPressedCallback(std::function<void(int button, int action, int mods)> Callback) {
        MouseButtonPressedCallback = Callback;
    }
}// namespace engine::glcore
