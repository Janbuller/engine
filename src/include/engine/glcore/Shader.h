#pragma once

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <glm/glm.hpp>

namespace engine::glcore {
    class Shader {
    private:
        const char *VertexPath;
        const char *FragmentPath;

    public:
        unsigned int ID;

        Shader() = default;
        Shader(const char *VertexPath, const char *FragmentPath);

        void LoadShaderFromFiles(const unsigned int ID, const char *VertexPath, const char *FragmentPath);

        void Bind() const;

        void SetBool(const std::string &name, bool value) const;
        void SetInt(const std::string &name, int value) const;
        void SetFloat(const std::string &name, float value) const;
        void SetVec2(const std::string &name, float x, float y) const;
        void SetVec3(const std::string &name, float x, float y, float z) const;
        void SetVec3(const std::string &name, const glm::vec3 &value) const;
        void SetVec3(const std::string &name, const float value[]) const;
        void SetMat4(const std::string &name, const glm::mat4 &mat) const;

        void Reload();
    };
}// namespace engine::glcore
