#pragma once
#include "engine/core/Application.h"
#include "engine/utils/DeltaVariable.h"
#include "engine/model/Mesh.h"
#include "engine/ecs/Scene.h"
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
        glcore::Shader MainShader{"res/shaders/MainShader.vert", "res/shaders/MainShader.frag"};

        engine::DeltaVariable<double, 2> RelativeMouse{std::array<double, 2>{0, 0}};

        engine::sptr<engine::Scene> MainScene = std::make_shared<engine::Scene>();

    private:
        void onCreate() override;
        bool onUpdate() override;
        void onExit() override;

        void DoInput(double deltaTime) {
            DoMouseInput();
        }

        void onKeyPressed(int key, int scancode, int action, int mods) override;
        void onMouseButtonPressed(int button, int action, int mods) override;

        void DoMouseInput();
    };
}// namespace ecstest
