#include "InputHandler.h"
#include <GLFW/glfw3.h>

InputHandler::InputHandler(GLFWwindow* window)
    : window(window)
{
    BindKey(Action::MoveForward,  GLFW_KEY_W);
    BindKey(Action::MoveBackward, GLFW_KEY_S);
    BindKey(Action::MoveLeft,     GLFW_KEY_A);
    BindKey(Action::MoveRight,    GLFW_KEY_D);
    BindKey(Action::Jump,         GLFW_KEY_SPACE);
    BindKey(Action::Pause,        GLFW_KEY_ESCAPE);

    double x;
    double y;
    glfwGetCursorPos(window, &x, &y);

    currentMousePosition = glm::vec2(x, y);
    lastMousePosition = currentMousePosition;
}

void InputHandler::Update()
{
    glfwPollEvents();

    double x;
    double y;
    glfwGetCursorPos(window, &x, &y);

    currentMousePosition = glm::vec2(x, y);

    if (firstMouseUpdate)
    {
        lastMousePosition = currentMousePosition;
        mouseDelta = glm::vec2(0.0f);
        firstMouseUpdate = false;
    }
    else
    {
        mouseDelta = currentMousePosition - lastMousePosition;
        lastMousePosition = currentMousePosition;
    }
}

void InputHandler::BindKey(Action action, int key)
{
    keyBindings[action] = key;
}

bool InputHandler::IsKeyDown(int key) const
{
    return glfwGetKey(window, key) == GLFW_PRESS;
}

bool InputHandler::IsActionDown(Action action) const
{
    auto it = keyBindings.find(action);

    if (it == keyBindings.end())
    {
        return false;
    }

    int key = it->second;
    return IsKeyDown(key);
}

bool InputHandler::IsMouseButtonDown(int button)
{
    return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

glm::vec2 InputHandler::GetMousePosition() const
{
    return currentMousePosition;
}

glm::vec2 InputHandler::GetMouseDelta() const
{
    return mouseDelta;
}