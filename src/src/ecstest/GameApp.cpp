#include "ecstest/GameApp.h"
#include "GLFW/glfw3.h"
#include "engine/core/Application.h"
#include "engine/core/Keys.h"
#include "engine/core/Logger.h"
#include "engine/ecs/Entity.h"
#include "engine/ecs/components/Camera.h"
#include "engine/ecs/components/Model.h"
#include "engine/ecs/components/Script.h"
#include "engine/ecs/components/Transform.h"
#include "engine/ecs/systems/LuaScriptRunner.h"
#include "engine/ecs/systems/ModelRenderer.h"
#include "engine/ecs/systems/MoveDown.h"
#include "engine/model/Mesh.h"
#include "engine/model/ModelLoader.h"
#include "engine/model/Vertex.h"
#include "engine/ressources/RessourceManager.h"
#include "engine/ressources/SpecificRessourceManager.h"
#include "glcore/Shader.h"
#include "glcore/Texture.h"
#include "glcore/Window.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/string_cast.hpp"
#include "siv/PerlinNoise.hpp"

// clang-format off
#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>
#include <LuaBridge/Vector.h>
// clang-format on

#include <bitset>
#include <functional>
#include <glad/glad.h>
#include <random>
#include <utility>
#include <vector>

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
        MainScene->RegisterComponentType<Camera>();

        MainScene->RegisterSystem<ModelRenderer>();
        auto ModelRendererSignature = std::bitset<engine::MAX_COMPONENTS>();
        ModelRendererSignature.set(MainScene->GetComponentId<Transform>(), true);
        ModelRendererSignature.set(MainScene->GetComponentId<Model>(), true);
        MainScene->SetSystemSignature<ModelRenderer>(ModelRendererSignature);

        MainScene->RegisterSystem<LuaScriptRunner>();
        auto LuaScriptRunnerSignature = std::bitset<engine::MAX_COMPONENTS>();
        LuaScriptRunnerSignature.set(MainScene->GetComponentId<Script>(), true);
        MainScene->SetSystemSignature<LuaScriptRunner>(LuaScriptRunnerSignature);

        for (int i = 0; i < 1; i++) {
            auto &E = MainScene->AddEntity();
            MainScene->AddComponent<Transform>(E);
            MainScene->AddComponent<Model>(E);
            MainScene->AddComponent<Script>(E);

            auto &ET = MainScene->GetComponent<Transform>(E);

            auto Scalar = 1.0;
            ET.Scale = {Scalar, Scalar, Scalar};

            auto &EModel = MainScene->GetComponent<Model>(E);
            EModel = engine::RessourceManager::Get<Model>("res/models/cube/cube.obj");

            // if (i == 0) {
            auto &EScript = MainScene->GetComponent<Script>(E);
            EScript.ScriptPaths.push_back("res/scripts/TestThingy.lua");
            // }
        }

        auto &Cam = MainScene->AddEntity();
        MainScene->AddComponent<Transform>(Cam);
        MainScene->AddComponent<Camera>(Cam);
        MainScene->AddComponent<Script>(Cam);

        auto &CamScript = MainScene->GetComponent<Script>(Cam);
        CamScript.ScriptPaths.push_back("res/scripts/CameraController.lua");

        MainScene->MainCam = Cam;

        MainScene->GetSystem<LuaScriptRunner>()->InitializeScripting(MainScene);

        std::function<bool(int)> IsKeyDownFn = [this](int KeyNum) {
            return AppWindow.GetKeyState(KeyNum) == glcore::Window::KeyState::KEY_PRESS;
        };
        MainScene->GetSystem<LuaScriptRunner>()->AddLuaFunction(IsKeyDownFn, "Input", "IsKeyDown");

        std::function<double(int)> GetMousePosFn = [this](int Axis) {
            auto MousePos = AppWindow.GetCursorPos();
            return MousePos[Axis - 1];
        };
        MainScene->GetSystem<LuaScriptRunner>()->AddLuaFunction(GetMousePosFn, "Input", "GetMousePos");

        std::function<std::vector<float>(std::vector<float>, float, std::vector<float>)> RotateFn =
                [this](std::vector<float> IQuat, float IAngle, std::vector<float> IAxis) {
                    glm::quat StartQuat{IQuat[0], IQuat[1], IQuat[2], IQuat[3]};
                    const auto &Angle = IAngle;
                    glm::vec3 Axis{IAxis[0], IAxis[1], IAxis[2]};

                    auto EndQuat = glm::rotate(StartQuat, Angle, Axis);
                    return std::vector<float>{EndQuat[0], EndQuat[1], EndQuat[2], EndQuat[3]};
                };
        MainScene->GetSystem<LuaScriptRunner>()->AddLuaFunction(RotateFn, "GMath", "Rotate");

        std::function<std::vector<float>(std::vector<float>, std::vector<float>)> RotatePointFn =
                [this](std::vector<float> IPoint, std::vector<float> IQuat) {
                    glm::quat Quat{IQuat[0], IQuat[1], IQuat[2], IQuat[3]};
                    glm::vec3 Point{IPoint[0], IPoint[1], IPoint[2]};

                    auto Rotated = glm::rotate(Quat, Point);
                    /* auto Rotated = Quat * Point; */
                    return std::vector<float>{Rotated[0], Rotated[1], Rotated[2]};
                };
        MainScene->GetSystem<LuaScriptRunner>()->AddLuaFunction(RotatePointFn, "GMath", "RotatePoint");

        std::function<std::vector<float>(std::vector<float>)> InverseQuatFn =
                [this](std::vector<float> IQuat) {
                    glm::quat Quat{IQuat[3], IQuat[0], IQuat[1], IQuat[2]};

                    auto Inverse = glm::inverse(Quat);
                    return std::vector<float>{Inverse[0], Inverse[1], Inverse[2], Inverse[3]};
                };
        MainScene->GetSystem<LuaScriptRunner>()->AddLuaFunction(InverseQuatFn, "GMath", "InverseQuat");

        MainScene->Init();
    }

    bool GameApp::onUpdate() {
        DoInput(DeltaTime);
        glClearColor(0.0, 0.3, 0.4, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        MainScene->GetSystem<ModelRenderer>()->Render(MainScene, AppWindow.Width, AppWindow.Height);

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

        MainScene->GetSystem<LuaScriptRunner>()->OnKeyPressed(MainScene, Keys(key), action);
    }

    void GameApp::onMouseButtonPressed(int button, int action, int mods) {
    }

    void GameApp::DoMouseInput() {
        // Get the relative mouse movement from the ~RelativeMouse~
        // DeltaVariable.
        auto [x, y] = RelativeMouse.GetDelta(AppWindow.GetCursorPos());

        /* MainCam.ProcessMouseMovement(x, -y, true); */
    }
}// namespace ecstest
