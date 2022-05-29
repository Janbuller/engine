#include "engine/Camera.h"

glm::mat4 engine::Camera::GetProjectionMatrix(int width, int height) {
  return glm::perspective(glm::radians(Zoom), (float) width / (float) height, 0.1f, 5000.0f);
}

glm::mat4 engine::Camera::GetViewMatrix() {
  return glm::lookAt(Position, Position + Front, Up);
}

void engine::Camera::ProcessKeyboard(MovDir direction, float deltaTime) {
  float velocity = MovementSpeed * deltaTime;
  if (direction == Camera::MovDir::FORWARD)
    Position += Front * velocity;
  if (direction == Camera::MovDir::BACKWARD)
    Position -= Front * velocity;
  if (direction == Camera::MovDir::LEFT)
    Position -= Right * velocity;
  if (direction == Camera::MovDir::RIGHT)
    Position += Right * velocity;

  updateCameraVectors();
}

void engine::Camera::ProcessMouseMovement(float xoffset, float yoffset, bool limitPitch = true) {
  xoffset *= MouseSensitivity;
  yoffset *= MouseSensitivity;

  Yaw += xoffset;
  Pitch += yoffset;

  if (limitPitch) {
    if (Pitch > 89.0f)
      Pitch = 89.0f;
    if (Pitch < -89.0f)
      Pitch = -89.0f;
  }

  updateCameraVectors();
}

void engine::Camera::updateCameraVectors() {
  glm::vec3 front;
  front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
  front.y = sin(glm::radians(Pitch));
  front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
  Front = glm::normalize(front);

  Right = glm::normalize(glm::cross(Front, WorldUp));
  Up = glm::normalize((glm::cross(Right, Front)));
}

void engine::Camera::SetSpeed(float speed) {
  MovementSpeed = speed;
}
