#include "ecstest/GameApp.h"
#include "GLFW/glfw3.h"
#include "engine/Application.h"
#include "engine/Camera.h"
#include "engine/Entity.h"
#include "engine/Mesh.h"
#include "engine/Vertex.h"
#include <bitset>
#include <glad/glad.h>
#include "engine/components/Transform.h"
#include "engine/systems/MoveDown.h"
#include "glcore/Shader.h"
#include "glcore/Window.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/string_cast.hpp"
#include "siv/PerlinNoise.hpp"
#include <functional>
#include <random>
#include <utility>
#include <vector>
#include "engine/Keys.h"
#include "engine/Logger.h"

namespace ecstest  {
  using namespace engine::components;
  using namespace engine::systems;
    void GameApp::onCreate() {
        AppWindow.CaptureMouse(true);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glFrontFace(GL_CW);

	// MainScene->Entities.push_back(engine::Entity{0, std::bitset<32>{}});
	std::default_random_engine Gen;
	std::uniform_real_distribution<float>  PosDist  {-30,  30};
	std::uniform_real_distribution<float>  RotDist  { 0,   6.283};
	std::uniform_real_distribution<double> ScaleDist{ 0.1, 4.0};

	MainScene->RegisterComponentType<Transform>();
	MainScene->RegisterSystem<MoveDown>();

	auto MoveDownSignature = std::bitset<engine::MAX_COMPONENTS>();
	MoveDownSignature.set(MainScene->GetComponentId<Transform>(), true);

	MainScene->SetSystemSignature<MoveDown>(MoveDownSignature);

	for(int i = 0; i < 2000; i++) {
	  auto& E = MainScene->AddEntity();
	  MainScene->AddComponent<Transform>(E);
	  auto& ET = MainScene->GetComponent<Transform>(E);
	  ET.Position = {PosDist(Gen), PosDist(Gen), PosDist(Gen)};
	  auto Scalar = ScaleDist(Gen);
	  ET.Scale    = {Scalar, Scalar, Scalar};

	  ET.Rotation = glm::rotate(ET.Rotation, RotDist(Gen), {1, 0, 0});
	  ET.Rotation = glm::rotate(ET.Rotation, RotDist(Gen), {0, 1, 0});
	  ET.Rotation = glm::rotate(ET.Rotation, RotDist(Gen), {0, 0, 1});
	}
    }

    bool GameApp::onUpdate() {
        DoInput(DeltaTime);
        glClearColor(0.0, 0.3, 0.4, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = MainCam.GetProjectionMatrix(AppWindow.width, AppWindow.height);
        glm::mat4 view = MainCam.GetViewMatrix();

        MainShader.Bind();
        MainShader.SetMat4("view", view);
        MainShader.SetMat4("projection", projection);

	for(int i = 0; i < 2000; i++) {
	  const auto& EntityTransform = MainScene->GetComponent<Transform>(MainScene->Entities[i]);
	  
	  glm::mat4 model = glm::mat4{1.0};
	  
	  model = glm::translate(model, EntityTransform.Position);
	  model = glm::scale(model, EntityTransform.Scale);
	  model *= glm::mat4_cast(EntityTransform.Rotation);
	  
	  MainShader.SetMat4("model", model);
	  
	  // Cube.Draw(MainShader);
	}

	MainScene->Update(DeltaTime);

        return true;
    }

    void GameApp::onKeyPressed(int key, int scancode, int action, int mods) {
        if (key == KEY_R && action == GLFW_PRESS)
            MainShader.Reload();
        if (key == KEY_ESCAPE && action == GLFW_PRESS)
            AppWindow.CaptureMouse(!AppWindow.IsMouseCaptured());

	if(key == KEY_LEFT_SHIFT && action == GLFW_PRESS) {
	  MainCam.SetSpeed(25);
	}
	if(key == KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
	  MainCam.SetSpeed(5);
	}
    }

    void GameApp::onMouseButtonPressed(int button, int action, int mods) {
    }

    void GameApp::DoKeyboardInput(double deltaTime) {
      using KeyState = glcore::Window::KeyState;
        if (AppWindow.GetKeyState(KEY_W) == KeyState::KEY_PRESS)
            MainCam.ProcessKeyboard(engine::Camera::MovDir::FORWARD, deltaTime);
        if (AppWindow.GetKeyState(KEY_S) == KeyState::KEY_PRESS)
            MainCam.ProcessKeyboard(engine::Camera::MovDir::BACKWARD, deltaTime);
        if (AppWindow.GetKeyState(KEY_A) == KeyState::KEY_PRESS)
            MainCam.ProcessKeyboard(engine::Camera::MovDir::LEFT, deltaTime);
        if (AppWindow.GetKeyState(KEY_D) == KeyState::KEY_PRESS)
            MainCam.ProcessKeyboard(engine::Camera::MovDir::RIGHT, deltaTime);


	for(int i = 0; i < 2000; i++) {
	  auto& Entity = MainScene->Entities[i];
	  auto& EntityTransform = MainScene->GetComponent<Transform>(Entity);
	  auto& EntPos = EntityTransform.Position;
	  auto& EntRot = EntityTransform.Rotation;
	  auto& EntScale = EntityTransform.Scale;

	  if(AppWindow.GetKeyState(KEY_UP) == KeyState::KEY_PRESS)
	    EntPos += glm::vec3{1, 0, 0} * EntRot * (float)DeltaTime * 10.0f;
	  if(AppWindow.GetKeyState(KEY_LEFT) == KeyState::KEY_PRESS)
	    EntRot = glm::rotate(EntRot, (float)-deltaTime, {0.0f, 1.0f, 0.0f});
	  if(AppWindow.GetKeyState(KEY_RIGHT) == KeyState::KEY_PRESS)
	    EntRot = glm::rotate(EntRot, (float)deltaTime, {0.0f, 1.0f, 0.0f});
	  if(AppWindow.GetKeyState(KEY_PAGE_DOWN) == KeyState::KEY_PRESS)
	    EntScale -= DeltaTime;
	  if(AppWindow.GetKeyState(KEY_PAGE_UP) == KeyState::KEY_PRESS)
	    EntScale += DeltaTime;
	}
    }

    void GameApp::DoMouseInput() {
        // Get the relative mouse movement from the ~RelativeMouse~
        // DeltaVariable.
        auto [x, y] = RelativeMouse.GetDelta(AppWindow.GetCursorPos());

        MainCam.ProcessMouseMovement(x, -y, true);
    }
}// namespace MarchingCubes
