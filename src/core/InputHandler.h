#pragma once

#include <glm/glm.hpp>
#include <unordered_map>

struct GLFWwindow;

enum class Action
{
    MoveForward,
    MoveBackward,
    MoveLeft,
    MoveRight,

    Jump,
    Descend,

    BreakBlock,
    PlaceBlock,

    Pause
};

class InputHandler
{
public:
    InputHandler(GLFWwindow* window);

    void BindKey(Action action, int key);

    bool IsActionDown(Action action) const;
    bool IsKeyDown(int key) const;

    bool IsMouseButtonDown(int button) const;

    glm::vec2 GetMousePosition() const;

    glm::vec2 GetMouseDelta() const;
    float GetScrollDelta() const;

    void Update();

private:
    GLFWwindow* window;
    std::unordered_map<Action, int> keyBindings;

    glm::vec2 currentMousePosition = glm::vec2(0.0f);
    glm::vec2 lastMousePosition = glm::vec2(0.0f);
    glm::vec2 mouseDelta = glm::vec2(0.0f);

    bool firstMouseUpdate = true;

    float scrollDelta = 0.0f;
    static InputHandler* scrollInstance;
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};