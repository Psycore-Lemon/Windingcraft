#include "scene/Camera.h"
#include <GLFW/glfw3.h>

Camera::Camera()
    : position(0.0f, 0.0f, 3.0f),
      front(0.0f, 0.0f, -1.0f),
      up(0.0f, 1.0f, 0.0f),
      movementSpeed(5.0f),
      fov(45.0f),
      nearPlane(0.1f),
      farPlane(100.0f)
{
}

void Camera::AttachToWindow(GLFWwindow* window)
{
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, MouseCallback);
}

void Camera::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    auto* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));

    if (cam->firstMouse)
    {
        cam->lastMouseX = (float)xpos;
        cam->lastMouseY = (float)ypos;
        cam->firstMouse = false;
    }

    float dx = (float)xpos - cam->lastMouseX;
    float dy = cam->lastMouseY - (float)ypos;
    cam->lastMouseX = (float)xpos;
    cam->lastMouseY = (float)ypos;

    cam->ProcessMouseMovement(dx, dy);
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) const
{
    return glm::perspective(
        glm::radians(fov),
        aspectRatio,
        nearPlane,
        farPlane
    );
}

void Camera::MoveForward(float dt)
{
    position += front * movementSpeed * dt;
}

void Camera::MoveBackward(float dt)
{
    position -= front * movementSpeed * dt;
}

void Camera::MoveLeft(float dt)
{
    glm::vec3 right = glm::normalize(glm::cross(front, up));
    position -= right * movementSpeed * dt;
}

void Camera::MoveRight(float dt)
{
    glm::vec3 right = glm::normalize(glm::cross(front, up));
    position += right * movementSpeed * dt;
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset)
{
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f)
        pitch = 89.0f;

    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front = glm::normalize(direction);
}

glm::vec3 Camera::GetFront() const
{
    return front;
}
