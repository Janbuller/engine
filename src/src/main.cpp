#include "ecstest/GameApp.h"
#include "glcore/Window.h"
#include "glcore/Shader.h"
#include "engine/Camera.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "engine/Mesh.h"
#include "engine/Application.h"

int main(int argc, char* argv[]) {

  std::cout << argv[0] << std::endl;
  ecstest::GameApp app{640, 480, "Hello, World!"};
  app.Run();

  return 0;
}
