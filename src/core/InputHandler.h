#pragma once

#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <unordered_map>

enum class Action
{
    MoveForward,
    MoveBackward,
    MoveLeft,
    MoveRight,

    Jump,

    Pause
};

class InputHandler
{
public:
    InputHandler(GLFWwindow* window);

    void BindKey(Action action, int key);

    bool IsActionDown(Action action) const;
    bool IsKeyDown(int key) const;

    bool IsMouseButtonDown(int button);

    glm::vec2 GetMousePosition() const;

    glm::vec2 GetMouseDelta() const;

    void Update();

private:
    GLFWwindow* window;
    std::unordered_map<Action, int> keyBindings;

    glm::vec2 currentMousePosition = glm::vec2(0.0f);
    glm::vec2 lastMousePosition = glm::vec2(0.0f);
    glm::vec2 mouseDelta = glm::vec2(0.0f);

    bool firstMouseUpdate = true;
};