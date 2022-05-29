#include "glcore/Window.h"
#include <cstdint>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <tuple>

namespace glcore {
  void Window::onWindowResize(int width, int height) {
    this->width = width;
    this->height = height;
    
    glViewport(0, 0, width, height);
  }

  void Window::onKeyPressed(int key, int scancode, int action, int mods) {
    if(KeyPressedCallback != nullptr) {
      KeyPressedCallback(key, scancode, action, mods);
    }
  }

  void Window::onMouseButtonPressed(int button, int action, int mods) {
    if(MouseButtonPressedCallback != nullptr) {
      MouseButtonPressedCallback(button, action, mods);
    }
  }

  Window::Window(int width, int height, std::string title) :
    width(width), height(height), title(title),
    deltaTime(std::array<double, 1>{ glfwGetTime() }){
    if (!glfwInit())
      throw -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!window) {
        glfwTerminate();
        throw -1;
    }

    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cerr << "Failed to initialize GLAD." << std::endl;
      throw -1;
    }

    glViewport(0, 0, width, height);
    lastFrame = glfwGetTime();

    // Use c++ lambda function as callback for glfw, forwarding it to
    // the onWindowResize function of this class, since glfw is a
    // c-library and doesn't support methods.

    glfwSetWindowUserPointer(window, this);

    auto windowSizeCallback = [](GLFWwindow* w, int width, int height)
    {
      static_cast<Window*>(glfwGetWindowUserPointer(w))->onWindowResize(width, height);
    };
    glfwSetWindowSizeCallback(window, windowSizeCallback);

    auto keyCallback = [](GLFWwindow* w, int key, int scancode, int action, int mods)
    {
      static_cast<Window*>(glfwGetWindowUserPointer(w))->onKeyPressed(key, scancode, action, mods);
    };
    glfwSetKeyCallback(window, keyCallback);

    auto mouseButtonCallback = [](GLFWwindow* w, int button, int action, int mods)
    {
      static_cast<Window*>(glfwGetWindowUserPointer(w))->onMouseButtonPressed(button, action, mods);
    };
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
  }

  Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void Window::SwapBuffers() {
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  void Window::Update() {
  }

  double Window::GetDeltaTime() {
    return deltaTime.GetDelta( {glfwGetTime()} )[0];
  }

  bool Window::ShouldClose() {
    return glfwWindowShouldClose(window);
  }

  void Window::SetShouldClose(bool close) {
    glfwSetWindowShouldClose(window, close);
  }
    

  void Window::CaptureMouse(bool capture) {
    if(capture)
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

  bool Window::IsMouseCaptured() {
    int inputMode = glfwGetInputMode(window, GLFW_CURSOR);
    return inputMode == GLFW_CURSOR_NORMAL ? false : true;
  }

  void Window::SetTitle(std::string title) {
    glfwSetWindowTitle(window, title.c_str());
  }

  Window::KeyState Window::GetKeyState(int key) {
    int state = glfwGetKey(window, key);

    if(state == GLFW_PRESS)
      return Window::KeyState::KEY_PRESS;
    if(state == GLFW_RELEASE)
      return Window::KeyState::KEY_RELEASE;

    return Window::KeyState::NONE;
  }

  std::array<double, 2> Window::GetCursorPos() {
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    return {x, y};
  }


  void Window::SetKeyPressedCallback(std::function<void(int key, int scandcode, int action, int mods)> Callback) {
    KeyPressedCallback = Callback;
  }

  void Window::SetMouseButtonPressedCallback(std::function<void(int button, int action, int mods)> Callback) {
    MouseButtonPressedCallback = Callback;
  }
} // namespace glcore
