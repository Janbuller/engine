#include "ecstest/GameApp.h"
#include "GLFW/glfw3.h"
#include "engine/Application.h"
#include "engine/Camera.h"
#include "engine/Entity.h"
#include "engine/Keys.h"
#include "engine/Logger.h"
#include "engine/Mesh.h"
#include "engine/ModelLoader.h"
#include "engine/RessourceManager.h"
#include "engine/Vertex.h"
#include "engine/components/Model.h"
#include "engine/components/Script.h"
#include "engine/components/Transform.h"
#include "engine/systems/LuaScriptRunner.h"
#include "engine/systems/ModelRenderer.h"
#include "engine/systems/MoveDown.h"
#include "glcore/Shader.h"
#include "glcore/Texture.h"
#include "glcore/Window.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/string_cast.hpp"
#include "siv/PerlinNoise.hpp"
#include <bitset>
#include <functional>
#include <glad/glad.h>
#include <random>
#include <utility>
#include <vector>
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>
#include <LuaBridge/Vector.h>
#include "engine/SpecificRessourceManager.h"

namespace ecstest {
    using namespace engine::components;
    using namespace engine::systems;
    void GameApp::onCreate() {
        AppWindow.CaptureMouse(true);

	engine::RessourceManager::RegisterRessourceType<Model>();
	engine::RessourceManager::RegisterRessourceType<glcore::Texture>();

        engine::ModelLoader::DefaultShader = MainShader;

        std::default_random_engine Gen;
        std::uniform_real_distribution<float> PosDist{-10, 100};
        std::uniform_real_distribution<float> RotDist{0, 6.283};
        std::uniform_real_distribution<double> ScaleDist{1.0, 1.0};

        MainScene->RegisterComponentType<Transform>();
        MainScene->RegisterComponentType<Model>();
        MainScene->RegisterComponentType<Script>();

        MainScene->RegisterSystem<ModelRenderer>();
        auto ModelRendererSignature = std::bitset<engine::MAX_COMPONENTS>();
        ModelRendererSignature.set(MainScene->GetComponentId<Transform>(), true);
        ModelRendererSignature.set(MainScene->GetComponentId<Model>(), true);
        MainScene->SetSystemSignature<ModelRenderer>(ModelRendererSignature);

        MainScene->RegisterSystem<LuaScriptRunner>();
        auto LuaScriptRunnerSignature = std::bitset<engine::MAX_COMPONENTS>();
        LuaScriptRunnerSignature.set(MainScene->GetComponentId<Script>(), true);
        MainScene->SetSystemSignature<LuaScriptRunner>(LuaScriptRunnerSignature);

        for (int i = 0; i < 10; i++) {
            auto &E = MainScene->AddEntity();
            MainScene->AddComponent<Transform>(E);
            MainScene->AddComponent<Model>(E);
            MainScene->AddComponent<Script>(E);

            auto &ET = MainScene->GetComponent<Transform>(E);
            // ET.Position = {PosDist(Gen), PosDist(Gen), PosDist(Gen)};

            // auto Scalar = ScaleDist(Gen);
            auto Scalar = 5.0;
            ET.Scale = {Scalar, Scalar, Scalar};

            // ET.Rotation = glm::rotate(ET.Rotation, RotDist(Gen), {1, 0, 0});
            // ET.Rotation = glm::rotate(ET.Rotation, RotDist(Gen), {0, 1, 0});
            // ET.Rotation = glm::rotate(ET.Rotation, RotDist(Gen), {0, 0, 1});

            auto &EModel = MainScene->GetComponent<Model>(E);
            EModel = engine::RessourceManager::Get<Model>("res/models/autumn_house_obj/house.obj");

            // if (i == 0) {
                auto &EScript = MainScene->GetComponent<Script>(E);
                EScript.ScriptPaths.push_back("res/scripts/TestThingy.lua");
            // }
        }

        MainScene->Init();

        std::function<bool(int)> IsKeyDownFn = [this](int KeyNum) {
            return AppWindow.GetKeyState(KeyNum) == glcore::Window::KeyState::KEY_PRESS;
        };
        MainScene->GetSystem<LuaScriptRunner>()->AddLuaFunction(IsKeyDownFn, "Input", "IsKeyDown");

        std::function<double(int)> GetMousePosFn = [this](int Axis) {
            auto MousePos = AppWindow.GetCursorPos();
            return MousePos[Axis - 1];
        };
        MainScene->GetSystem<LuaScriptRunner>()->AddLuaFunction(GetMousePosFn, "Input", "GetMousePos");

        std::function<std::vector<float>(std::vector<float>, float, std::vector<float>)> RotateFn = [this](std::vector<float> IQuat, float IAngle, std::vector<float> IAxis) {
            glm::quat StartQuat{IQuat[0], IQuat[1], IQuat[2], IQuat[3]};
            const auto &Angle = IAngle;
            glm::vec3 Axis{IAxis[0], IAxis[1], IAxis[2]};

            auto EndQuat = glm::rotate(StartQuat, Angle, Axis);
            return std::vector<float>{EndQuat[0], EndQuat[1], EndQuat[2], EndQuat[3]};
        };

        MainScene->GetSystem<LuaScriptRunner>()->AddLuaFunction(RotateFn, "GMath", "Rotate");
    }

    bool GameApp::onUpdate() {
        DoInput(DeltaTime);
        glClearColor(0.0, 0.3, 0.4, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = MainCam.GetProjectionMatrix(AppWindow.width, AppWindow.height);
        glm::mat4 view = MainCam.GetViewMatrix();

        MainScene->GetSystem<ModelRenderer>()->Render(MainScene, view, projection);

        MainScene->Update(DeltaTime);

        return true;
    }

    void GameApp::onExit() {
        MainScene->Exit();
    }

    void GameApp::onKeyPressed(int key, int scancode, int action, int mods) {
        using namespace engine;
        if (key == KEY_R && action == GLFW_PRESS)
            MainShader.Reload();
        if (key == KEY_ESCAPE && action == GLFW_PRESS)
            AppWindow.CaptureMouse(!AppWindow.IsMouseCaptured());

        if (key == KEY_LEFT_SHIFT && action == GLFW_PRESS) {
            MainCam.SetSpeed(125);
        }
        if (key == KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
            MainCam.SetSpeed(5);
        }

        MainScene->GetSystem<LuaScriptRunner>()->OnKeyPressed(MainScene, Keys(key), action);
    }

    void GameApp::onMouseButtonPressed(int button, int action, int mods) {
    }

    void GameApp::DoKeyboardInput(double deltaTime) {
        using namespace engine;
        using KeyState = glcore::Window::KeyState;
        if (AppWindow.GetKeyState(KEY_W) == KeyState::KEY_PRESS)
            MainCam.ProcessKeyboard(engine::Camera::MovDir::FORWARD, deltaTime);
        if (AppWindow.GetKeyState(KEY_S) == KeyState::KEY_PRESS)
            MainCam.ProcessKeyboard(engine::Camera::MovDir::BACKWARD, deltaTime);
        if (AppWindow.GetKeyState(KEY_A) == KeyState::KEY_PRESS)
            MainCam.ProcessKeyboard(engine::Camera::MovDir::LEFT, deltaTime);
        if (AppWindow.GetKeyState(KEY_D) == KeyState::KEY_PRESS)
            MainCam.ProcessKeyboard(engine::Camera::MovDir::RIGHT, deltaTime);


        for (int i = 0; i < 1; i++) {
            auto &Entity = MainScene->Entities[i];
            auto &EntityTransform = MainScene->GetComponent<Transform>(Entity);
            auto &EntPos = EntityTransform.Position;
            auto &EntRot = EntityTransform.Rotation;
            auto &EntScale = EntityTransform.Scale;

            if (AppWindow.GetKeyState(KEY_UP) == KeyState::KEY_PRESS)
                EntPos += glm::vec3{1, 0, 0} * EntRot * (float) DeltaTime * 10.0f;
            if (AppWindow.GetKeyState(KEY_LEFT) == KeyState::KEY_PRESS)
                EntRot = glm::rotate(EntRot, (float) -deltaTime, {0.0f, 1.0f, 0.0f});
            if (AppWindow.GetKeyState(KEY_RIGHT) == KeyState::KEY_PRESS)
                EntRot = glm::rotate(EntRot, (float) deltaTime, {0.0f, 1.0f, 0.0f});
            if (AppWindow.GetKeyState(KEY_PAGE_DOWN) == KeyState::KEY_PRESS)
                EntScale -= DeltaTime;
            if (AppWindow.GetKeyState(KEY_PAGE_UP) == KeyState::KEY_PRESS)
                EntScale += DeltaTime;
        }
    }

    void GameApp::DoMouseInput() {
        // Get the relative mouse movement from the ~RelativeMouse~
        // DeltaVariable.
        auto [x, y] = RelativeMouse.GetDelta(AppWindow.GetCursorPos());

        MainCam.ProcessMouseMovement(x, -y, true);
    }
}// namespace ecstest
