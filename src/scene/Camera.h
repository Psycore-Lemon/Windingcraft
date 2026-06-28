#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    Camera();

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float aspectRatio) const;

    glm::vec3 position;

    void MoveForward(float dt);
    void MoveBackward(float dt);
    void MoveLeft(float dt);
    void MoveRight(float dt);

    void ProcessMouseMovement(float xOffset, float yOffset);

private:
    glm::vec3 front;
    glm::vec3 up;

    float movementSpeed;

    float fov;
    float nearPlane;
    float farPlane;

    float yaw = -90.0f;
    float pitch = 0.0f;
    float mouseSensitivity = 0.1f;
};