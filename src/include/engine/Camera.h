#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

namespace engine {
    class Camera {
    private:
        // Default values
        const float YAW = -90.0f;
        const float PITCH = 0.0f;
        const float SPEED = 5.0f;
        const float SENSITIVITY = 0.05f;
        const float ZOOM = 60.0f;

    public:
        enum class MovDir { FORWARD,
                            BACKWARD,
                            LEFT,
                            RIGHT };

        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;

        float Yaw = YAW;
        float Pitch = PITCH;

        float MovementSpeed;
        float MouseSensitivity;
        float Zoom;

        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
               glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f))
            : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
              MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {

            Position = position;
            WorldUp = up;
            updateCameraVectors();
        }
        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ)
            : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED),
              MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {

            Position = glm::vec3(posX, posY, posZ);
            WorldUp = glm::vec3(upX, upY, upZ);
            updateCameraVectors();
        }

      glm::mat4 GetProjectionMatrix(int width, int height);
        glm::mat4 GetViewMatrix();

        void ProcessKeyboard(MovDir direction, float deltaTime);

        void ProcessMouseMovement(float xoffset, float yoffset, bool limitPitch);

        void SetSpeed(float speed);

    private:
        void updateCameraVectors();
    };
}// namespace engine
