#include "engine/glcore/Shader.h"
#include "engine/Base.h"

namespace engine::glcore {
    Shader::Shader(const char *VertexPath, const char *FragmentPath) {
        ID = glCreateProgram();
        LoadShaderFromFiles(ID, VertexPath, FragmentPath);
    }

    void Shader::LoadShaderFromFiles(const unsigned int ID, const char *VertexPath, const char *FragmentPath) {
        this->VertexPath   = VertexPath;
        this->FragmentPath = FragmentPath;

        std::ifstream VertexShaderFile;
        std::ifstream FragmentShaderFile;

        VertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        FragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        std::stringstream VertexShaderStream;
        std::stringstream FragmentShaderStream;

        try {
            VertexShaderFile.open(VertexPath);
            VertexShaderStream << VertexShaderFile.rdbuf();
            VertexShaderFile.close();
        } catch (std::ifstream::failure &e) {
            LOG_ENGINE_ERROR("Error loading shader file {0}! Error: {1}", VertexPath, e.what());
            throw e;
        }
        try {
            FragmentShaderFile.open(FragmentPath);
            FragmentShaderStream << FragmentShaderFile.rdbuf();
            FragmentShaderFile.close();
        } catch (std::ifstream::failure &e) {
            LOG_ENGINE_ERROR("Error loading shader file {0}! Error: {1}", FragmentPath, e.what());
            throw e;
        }

        const auto VertexCode   = VertexShaderStream.str();
        const auto FragmentCode = FragmentShaderStream.str();

        const auto *VertexShaderCode   = VertexCode.c_str();
        const auto *FragmentShaderCode = FragmentCode.c_str();

        unsigned int VertexID, FragmentID;
        int Success;
        unsigned int InfoLogLen = 1024;
        char InfoLog[InfoLogLen];

        VertexID = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(VertexID, 1, &VertexShaderCode, NULL);
        glCompileShader(VertexID);

        glGetShaderiv(VertexID, GL_COMPILE_STATUS, &Success);
        if (!Success) {
            glGetShaderInfoLog(VertexID, InfoLogLen, NULL, InfoLog);
            LOG_ENGINE_ERROR("Vertex shader compilation has failed with the error message:\n{0}", &InfoLog[0]);
        }

        FragmentID = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(FragmentID, 1, &FragmentShaderCode, NULL);
        glCompileShader(FragmentID);

        glGetShaderiv(FragmentID, GL_COMPILE_STATUS, &Success);
        if (!Success) {
            glGetShaderInfoLog(FragmentID, InfoLogLen, NULL, InfoLog);
            LOG_ENGINE_ERROR("Fragment shader compilation has failed with the error message:\n{0}", &InfoLog[0]);
        }

        glAttachShader(ID, VertexID);
        glAttachShader(ID, FragmentID);
        glLinkProgram(ID);

        glGetProgramiv(ID, GL_LINK_STATUS, &Success);
        if (!Success) {
            glGetProgramInfoLog(ID, 512, NULL, InfoLog);
            LOG_ENGINE_ERROR("Shader program linking has failed with the error message:\n{0}", &InfoLog[0]);
        } else {
            LOG_ENGINE_TRACE("Shader load from files: (\"{0}\") & (\"{1}\")", VertexPath, FragmentPath);
        }

        glDetachShader(ID, VertexID);
        glDetachShader(ID, FragmentID);

        glDeleteShader(VertexID);
        glDeleteShader(FragmentID);
    }
    void Shader::Bind() const { glUseProgram(ID); }

    void Shader::SetBool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int) value);
    }
    void Shader::SetInt(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    void Shader::SetFloat(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void Shader::SetVec2(const std::string &name, float x, float y) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    void Shader::SetVec3(const std::string &name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    void Shader::SetVec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void Shader::SetVec3(const std::string &name, const float value[]) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void Shader::SetMat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                           &mat[0][0]);
    }
    void Shader::Reload() {
        LoadShaderFromFiles(ID, VertexPath, FragmentPath);
    }
}// namespace engine::glcore
