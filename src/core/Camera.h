#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 position;
    float yaw   = -90.0f;  // facing -Z
    float pitch = 0.0f;
    float fov   = 45.0f;
    float speed = 5.0f;       // units/sec
    float sensitivity = 0.1f;

    Camera(glm::vec3 startPos = {0.0f, 0.0f, 3.0f}) : position(startPos) {}

    glm::mat4 view() const {
        return glm::lookAt(position, position + front(), worldUp);
    }
    glm::mat4 projection(float aspect) const {
        return glm::perspective(glm::radians(fov), aspect, 0.1f, 1000.0f);
    }

    glm::vec3 front() const {
        glm::vec3 f;
        f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        f.y = sin(glm::radians(pitch));
        f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        return glm::normalize(f);
    }
    glm::vec3 right() const { return glm::normalize(glm::cross(front(), worldUp)); }

    // call from your input handler each frame
    void processKeyboard(int key, float dt) {
        float v = speed * dt;
        if (key == GLFW_KEY_W) position += front() * v;
        if (key == GLFW_KEY_S) position -= front() * v;
        if (key == GLFW_KEY_A) position -= right() * v;
        if (key == GLFW_KEY_D) position += right() * v;
    }
    void processMouse(float dx, float dy) {
        yaw   += dx * sensitivity;
        pitch += dy * sensitivity;
        pitch = glm::clamp(pitch, -89.0f, 89.0f);  // no gimbal flip
    }

private:
    glm::vec3 worldUp{0.0f, 1.0f, 0.0f};
};