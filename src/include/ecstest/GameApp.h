#pragma once
#include "engine/Application.h"
#include "engine/Camera.h"
#include "engine/DeltaVariable.h"
#include "engine/Mesh.h"
#include "engine/Scene.h"
#include "glcore/Shader.h"
#include "glcore/Texture.h"
#include <algorithm>
#include <array>
#include <map>
#include <memory>


namespace ecstest {
    class GameApp : public engine::Application {

        using engine::Application::Application;

    private:
        // engine::Mesh cubeMesh = engine::Mesh::LoadOBJ("cube.obj", "res/");
        glcore::Shader MainShader{"res/shaders/MainShader.vert", "res/shaders/MainShader.frag"};

        engine::DeltaVariable<double, 2> RelativeMouse{std::array<double, 2>{0, 0}};

        engine::Camera MainCam{};

      // engine::Mesh Cube = engine::Mesh::FromRawMesh(engine::RawMesh::LoadOBJ("cube.obj", "res/"), {glcore::Texture::LoadTextureFromFile("res/texture.png")});

      engine::sptr<engine::Scene> MainScene = std::make_shared<engine::Scene>();
    private:
        void onCreate() override;
        bool onUpdate() override;

        void DoInput(double deltaTime) {
            DoKeyboardInput(deltaTime);
            DoMouseInput();
        }

        void onKeyPressed(int key, int scancode, int action, int mods) override;
        void onMouseButtonPressed(int button, int action, int mods) override;

        void DoKeyboardInput(double deltaTime);
        void DoMouseInput();
    };
}// namespace MarchingCubes
