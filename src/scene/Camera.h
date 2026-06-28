#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct GLFWwindow;

class Camera
{
public:
    Camera();

    void AttachToWindow(GLFWwindow* window);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float aspectRatio) const;

    glm::vec3 position;

    void MoveForward(float dt);
    void MoveBackward(float dt);
    void MoveLeft(float dt);
    void MoveRight(float dt);

    void ProcessMouseMovement(float xOffset, float yOffset);

    glm::vec3 GetFront() const;

private:
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);

    glm::vec3 front;
    glm::vec3 up;

    float movementSpeed;

    float fov;
    float nearPlane;
    float farPlane;

    float yaw = -90.0f;
    float pitch = 0.0f;
    float mouseSensitivity = 0.1f;

    float lastMouseX = 0.0f;
    float lastMouseY = 0.0f;
    bool firstMouse = true;
};