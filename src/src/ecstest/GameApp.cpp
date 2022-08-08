#include "ecstest/GameApp.h"
#include "GLFW/glfw3.h"
#include "engine/core/Application.h"
#include "engine/core/Keys.h"
#include "engine/ecs/Entity.h"
#include "engine/ecs/components/Camera.h"
#include "engine/ecs/components/Light.h"
#include "engine/ecs/components/Model.h"
#include "engine/ecs/components/Script.h"
#include "engine/ecs/components/Transform.h"
#include "engine/ecs/systems/LuaScriptRunner.h"
#include "engine/ecs/systems/ModelRenderer.h"
#include "engine/glcore/Shader.h"
#include "engine/glcore/TextureCubemap.h"
#include "engine/glcore/Window.h"
#include "engine/model/ModelLoader.h"
#include "engine/ressources/RessourceManager.h"
#include "glm/ext/quaternion_transform.hpp"
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
        engine::RessourceManager::RegisterRessourceType<engine::glcore::TextureData>();

        engine::ModelLoader::DefaultShader = MainShader;

        MainScene->RegisterComponentType<Transform>();
        MainScene->RegisterComponentType<Model>();
        MainScene->RegisterComponentType<Script>();
        MainScene->RegisterComponentType<Camera>();
        MainScene->RegisterComponentType<Light>();

        MainScene->RegisterSystem<ModelRenderer>();
        auto ModelRendererSignature = std::bitset<engine::MAX_COMPONENTS>();
        ModelRendererSignature.set(MainScene->GetComponentId<Transform>(), true);
        ModelRendererSignature.set(MainScene->GetComponentId<Model>(), true);
        MainScene->SetSystemSignature<ModelRenderer>(ModelRendererSignature);

        MainScene->RegisterSystem<LuaScriptRunner>();
        auto LuaScriptRunnerSignature = std::bitset<engine::MAX_COMPONENTS>();
        LuaScriptRunnerSignature.set(MainScene->GetComponentId<Script>(), true);
        MainScene->SetSystemSignature<LuaScriptRunner>(LuaScriptRunnerSignature);

        // Create floor entity
        {
            auto &E  = MainScene->AddEntity();
            auto &ET = MainScene->AddComponent<Transform>(E);
            auto &EM = MainScene->AddComponent<Model>(E);
            auto &ES = MainScene->AddComponent<Script>(E);

            ET.Position.y -= 2;

            EM = engine::RessourceManager::Get<Model>("res/Application/Models/pavement_floor/floor.fbx");

            ES.ScriptPaths.push_back("res/Application/Scripts/TestThingy.lua");
        }

        // Create sun light entity
        {
            auto &E  = MainScene->AddEntity();
            auto &ET = MainScene->AddComponent<Transform>(E);
            auto &EL = MainScene->AddComponent<Light>(E);

            ET.Rotation = glm::rotate(ET.Rotation, 0.66f, glm::vec3{1, 0, 0});
            ET.Rotation = glm::rotate(ET.Rotation, 0.4f, glm::vec3{0, 0, 1});

            EL.Color     = {0.9, 0.9, 1.0};
            EL.Intensity = 0.2;

            EL.Constant  = 1.0;
            EL.Linear    = 0.35;
            EL.Quadratic = 0.44;

            EL.Type = Light::LightType::DirectionalLight;
        }

        // Create sun light entity
        {
            auto &E  = MainScene->AddEntity();
            auto &ET = MainScene->AddComponent<Transform>(E);
            auto &EL = MainScene->AddComponent<Light>(E);

            ET.Rotation = glm::rotate(ET.Rotation, -0.66f, glm::vec3{1, 0, 0});
            ET.Rotation = glm::rotate(ET.Rotation, 0.4f, glm::vec3{0, 0, 1});

            EL.Color     = {0.9, 0.9, 1.0};
            EL.Intensity = 0.2;

            EL.Constant  = 1.0;
            EL.Linear    = 0.35;
            EL.Quadratic = 0.44;

            EL.Type = Light::LightType::DirectionalLight;
        }

        // Create point light entity
        {
            auto &E  = MainScene->AddEntity();
            auto &ET = MainScene->AddComponent<Transform>(E);
            auto &EL = MainScene->AddComponent<Light>(E);

            ET.Position = {-1, 1, 0};

            EL.Color     = {1, 0.97, 0.94};
            EL.Intensity = 5.0;

            EL.Constant  = 1.0;
            EL.Linear    = 0.14;
            EL.Quadratic = 0.07;

            EL.Type = Light::LightType::PointLight;
        }

        // Create camera entity
        {
            auto &Cam = MainScene->AddEntity();
            auto &CT  = MainScene->AddComponent<Transform>(Cam);
            auto &CC  = MainScene->AddComponent<Camera>(Cam);
            auto &CS  = MainScene->AddComponent<Script>(Cam);

            /* CC.Projection = engine::components::Camera::ProjectionType::ORTHOGRAPHIC; */
            CS.ScriptPaths.push_back("res/Application/Scripts/CameraController.lua");

            // Cubemap loading
            {
                using namespace engine;
                using namespace engine::glcore;
                auto CubeMap = TextureCubemap::FromTextureData(
                        {RessourceManager::Get<TextureData>("res/Application/Skybox/Evening Meadow/2048/xp.jpg"),
                         RessourceManager::Get<TextureData>("res/Application/Skybox/Evening Meadow/2048/xn.jpg"),
                         RessourceManager::Get<TextureData>("res/Application/Skybox/Evening Meadow/2048/yp.jpg"),
                         RessourceManager::Get<TextureData>("res/Application/Skybox/Evening Meadow/2048/yn.jpg"),
                         RessourceManager::Get<TextureData>("res/Application/Skybox/Evening Meadow/2048/zp.jpg"),
                         RessourceManager::Get<TextureData>("res/Application/Skybox/Evening Meadow/2048/zn.jpg")});
                CC.Skybox = CubeMap;
            }
            CC.BackgroundColor = glm::vec4{0.0, 0.3, 0.4, 1.0};

            MainScene->MainCam = Cam;
        }

        MainScene->GetSystem<LuaScriptRunner>()->InitializeScripting(MainScene);

        std::function<bool(int)> IsKeyDownFn = [this](int KeyNum) {
            return AppWindow.GetKeyState(KeyNum) == engine::glcore::Window::KeyState::KEY_PRESS;
        };
        MainScene->GetSystem<LuaScriptRunner>()->AddLuaFunction(IsKeyDownFn, "Input", "IsKeyDown");

        std::function<double(int)> GetMousePosFn = [this](int Axis) {
            auto MousePos = AppWindow.GetCursorPos();
            return MousePos[Axis - 1];
        };
        MainScene->GetSystem<LuaScriptRunner>()->AddLuaFunction(GetMousePosFn, "Input", "GetMousePos");

        MainScene->Init();
    }

    bool GameApp::onUpdate() {
        DoInput(DeltaTime);

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

        if(key == KEY_F1 && action == GLFW_PRESS)
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        if(key == KEY_F2 && action == GLFW_PRESS)
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

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
